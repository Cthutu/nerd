#!/usr/bin/env python3
"""Estimate optimistic module-check parallelism from saved serial profiles.

This is a fixed-weight DAG model, not a parallel compiler benchmark. It ignores
worker limits, shared mutation, transfer costs and unrecorded implicit edges.
"""
import argparse
import gzip
import json
from pathlib import Path


def critical_path(weights, edges):
    dependencies = {module: set() for module in weights}
    for module, dependency in edges:
        if module not in weights or dependency not in weights:
            raise ValueError(f'Missing semantic phase for dependency: {module} -> {dependency}')
        dependencies[module].add(dependency)
    done, visiting = {}, set()

    def visit(module):
        if module in visiting:
            raise ValueError(f'Cycle in dependency profile at {module}')
        if module not in done:
            visiting.add(module)
            done[module] = weights[module] + max((visit(d) for d in dependencies[module]), default=0)
            visiting.remove(module)
        return done[module]

    return max((visit(module) for module in weights), default=0)


def analyse(data):
    result = []
    for row in data['runs']:
        serial = row['jobs']['1']
        weights = {}
        for record in serial['profile']:
            if record.get('kind') == 'phase' and record.get('phase') == 'analyse AST semantics':
                module = record['module']
                if module in weights:
                    raise ValueError(f'Duplicate semantic phase for {module}')
                weights[module] = record['wall_ns']
        if not weights:
            raise ValueError('No serial semantic phases found')
        edges = [(r['module'], r['dependency']) for r in serial['profile'] if r['kind'] == 'dependency']
        total, critical = sum(weights.values()), critical_path(weights, edges)
        saved = total - critical
        phases = [r for r in serial['profile'] if r.get('kind') == 'phase']
        start = min(r['start_ns'] for r in phases if r.get('stage') == 'front-end')
        end = max(r['start_ns'] + r['wall_ns'] for r in phases if r.get('phase') == 'combine LLVM text')
        result.append(dict(optimistic_source_to_ir_gain_percent=100 * saved / (end - start),
                           scenario=row['scenario'], target=row['target'], modules=len(weights),
                           semantic_serial_ns=total, semantic_critical_path_ns=critical,
                           optimistic_saved_ns=saved,
                           optimistic_whole_profile_gain_percent=100 * saved / serial['profile_sample']['wall_ns']))
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    payload = args.input.read_bytes()
    if args.input.suffix == '.gz':
        payload = gzip.decompress(payload)
    results = analyse(json.loads(payload))
    args.output.write_text(json.dumps({'input': str(args.input),
        'model': 'unlimited workers, fixed serial module weights, recorded dependency edges only',
        'runs': results}, indent=2) + '\n')
    for r in results:
        print(f"{r['scenario']:8} {r['target']:7}: sema {r['semantic_serial_ns']/1e6:.2f} ms, "
              f"critical {r['semantic_critical_path_ns']/1e6:.2f} ms, "
              f"optimistic whole-profile gain {r['optimistic_whole_profile_gain_percent']:.2f}%")


if __name__ == '__main__':
    main()
