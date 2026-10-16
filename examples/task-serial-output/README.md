# Concurrent calculations with serial output

Run `just run-example task-serial-output` from the repository root.

Eight concurrent tasks square the numbers 1 through 8. Each submits its own
output task to a serial worker queue. Output callbacks execute one at a time,
print their result and update a shared summary without a separate mutex.
The individual lines can appear in any input order; the final line is always
`Serial output: total = 204`.

The owner waits for calculations before waiting for outputs, including on early
return. This keeps both task sets and their borrowed contexts alive. The finite
example reserves enough capacity for every calculation and output task.

See the [Raptor tutorial](../../docs/tutorials/raptor.md#concurrent-calculations-feeding-serial-output)
and [queue contracts](../../docs/stdlib-raptor.md).
