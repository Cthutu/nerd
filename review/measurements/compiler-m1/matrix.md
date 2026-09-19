# M1 benchmark matrix

Milliseconds; warm medians use five unprofiled samples, cold-input advice uses three.

| Scenario | Mode | Target | Warm median | Warm min–max | Cold-input advice median | Peak process RSS MiB (warm max) |
| --- | --- | --- | ---: | ---: | ---: | ---: |
| tiny | check | debug | 1.51 | 1.35–1.63 | 1.73 | 22.3 |
| tiny | check | release | 1.31 | 1.23–1.44 | 1.64 | 22.3 |
| tiny | llvm | debug | 32.34 | 31.65–32.36 | 36.00 | 50.2 |
| tiny | llvm | release | 38.14 | 37.68–39.09 | 39.46 | 47.7 |
| tiny | cgen | debug | 2.59 | 2.30–2.69 | 2.64 | 22.3 |
| tiny | cgen | release | 2.59 | 2.19–2.68 | 3.35 | 22.3 |
| dungeon | check | debug | 300.48 | 298.99–304.72 | 312.06 | 22.3 |
| dungeon | check | release | 300.29 | 299.48–304.97 | 312.33 | 22.3 |
| dungeon | llvm | debug | 3030.44 | 3008.36–3050.97 | 3098.10 | 73.9 |
| dungeon | llvm | release | 4571.12 | 4512.10–4719.57 | 4633.14 | 145.8 |
| dungeon | cgen | debug | 443.65 | 436.10–446.59 | 421.58 | 22.3 |
| dungeon | cgen | release | 430.20 | 425.70–432.98 | 424.83 | 22.3 |
| pixels | check | debug | 347.82 | 345.61–355.08 | 357.08 | 22.3 |
| pixels | check | release | 337.96 | 332.30–345.37 | 349.70 | 22.3 |
| pixels | llvm | debug | 1116.34 | 1095.50–1137.88 | 1113.56 | 61.2 |
| pixels | llvm | release | 1072.25 | 1059.08–1084.39 | 1048.17 | 66.0 |
| pixels | cgen | debug | 464.85 | 461.01–466.44 | 468.96 | 23.1 |
| pixels | cgen | release | 462.14 | 455.47–467.59 | 467.02 | 23.1 |
| quill | check | debug | 28.89 | 28.69–29.96 | 31.35 | 23.3 |
| quill | check | release | 27.45 | 27.21–28.20 | 30.39 | 23.6 |
| quill | llvm | debug | 185.42 | 184.94–186.10 | 187.06 | 54.3 |
| quill | llvm | release | 169.98 | 159.53–177.68 | 184.53 | 60.0 |
| quill | cgen | debug | 41.55 | 40.61–47.59 | 42.65 | 24.1 |
| quill | cgen | release | 42.25 | 38.93–44.04 | 50.86 | 24.3 |
| wide | check | debug | 18.25 | 16.12–19.29 | 19.75 | 24.3 |
| wide | check | release | 18.03 | 16.68–19.55 | 20.03 | 24.6 |
| wide | llvm | debug | 274.18 | 245.60–276.68 | 251.35 | 54.9 |
| wide | llvm | release | 240.79 | 236.69–244.40 | 259.43 | 50.3 |
| wide | cgen | debug | 26.80 | 26.57–27.11 | 28.08 | 25.6 |
| wide | cgen | release | 26.74 | 25.69–27.49 | 29.43 | 25.8 |
| deep | check | debug | 19.88 | 19.28–20.14 | 19.07 | 26.1 |
| deep | check | release | 19.65 | 19.32–20.69 | 22.42 | 26.3 |
| deep | llvm | debug | 263.89 | 231.25–266.93 | 266.28 | 55.0 |
| deep | llvm | release | 249.24 | 236.64–257.53 | 263.28 | 50.2 |
| deep | cgen | debug | 27.00 | 26.48–29.02 | 25.04 | 27.3 |
| deep | cgen | release | 25.56 | 24.20–25.64 | 28.23 | 27.6 |
| large | check | debug | 204.41 | 198.17–207.58 | 209.46 | 27.8 |
| large | check | release | 196.86 | 192.56–203.98 | 206.47 | 27.8 |
| large | llvm | debug | 530.23 | 509.00–542.93 | 500.43 | 54.4 |
| large | llvm | release | 437.52 | 434.02–441.09 | 436.67 | 47.7 |
| large | cgen | debug | 217.19 | 215.51–220.75 | 227.56 | 28.0 |
| large | cgen | release | 209.04 | 193.63–214.54 | 226.76 | 28.0 |
