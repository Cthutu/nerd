#!/usr/bin/env python3
"""Validate semantic critical-path analysis using graphs with known schedules."""
from semantic_headroom import critical_path

assert critical_path({}, []) == 0
assert critical_path({'a': 3, 'b': 5}, []) == 5
assert critical_path({'a': 3, 'b': 5}, [('a', 'b')]) == 8
# The shared leaf is checked once and the two branches can overlap.
weights = {'root': 1, 'left': 3, 'right': 5, 'leaf': 2}
edges = [('root', 'left'), ('root', 'right'), ('left', 'leaf'), ('right', 'leaf')]
assert critical_path(weights, edges) == 8
assert critical_path(weights, edges + [('root', 'left')]) == 8
for bad in [[('root', 'missing')], edges + [('leaf', 'root')]]:
    try:
        critical_path(weights, bad)
    except ValueError:
        pass
    else:
        raise AssertionError(bad)
print('Semantic critical-path model: independent, chain, diamond and invalid graphs passed')
