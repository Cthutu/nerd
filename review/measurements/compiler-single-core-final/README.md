# Final single-core evidence

The [report](../compiler-single-core-final.md) interprets these results. JSON,
logs and snapshot scripts are gzip-compressed; use `gzip -dc FILE.gz` to inspect.
`provenance.json.gz` records compiler hashes, tool versions, affinity commands
and the implementation commit. The original baseline is `a15e965d`; this pass
starts at `990cfb5a` and finishes at `6dd4b36e`.

- `paired`: seven samples per compiler/target, three separate profile samples.
- `original`: seven samples per compiler/check-or-build mode.
- `workers`: five samples for each numeric/automatic worker setting.
- `scale`: three samples for each compiler/worker variant and one profile sample;
  all source hashes, resource samples and profiling records are retained.
- Validation logs cover fixtures, integrations, ASan, TSan and Linux unit tests
  for the Windows runner. They do not establish native Windows/macOS success.

The repository's `build/benchmark_compare.py`, `build/benchmark_main.py` and
`build/benchmark_jobs.py` supply the first three harnesses. The archived
`benchmarks.py` and `scale.py` are exact run snapshots containing absolute paths;
extract them into a temporary directory and adjust paths/CPU affinity to repeat
elsewhere. Rebuild the recorded compiler revisions rather than substituting a
globally installed compiler. Timings will depend on the host and tool versions.

To recreate only the large inputs in a temporary directory:

```sh
work=$(mktemp -d)
gzip -dc generate_scale.py.gz > "$work/generate_scale.py"
python "$work/generate_scale.py" "$work/inputs"
```

The generator emits 50/200/1,000-unit wide, shared, chain and heavy shapes.
Each unit contributes 24 functions; heavy distributes them over four modules.
The scaled runner compiles each variant, checks LLVM hashes and executes every
binary before accepting a sample. Its `timeout 240` applies to each compilation.
No generated source trees or executables are committed here.
