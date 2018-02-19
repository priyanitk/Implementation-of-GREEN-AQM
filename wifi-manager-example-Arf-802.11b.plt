set terminal postscript eps color enh "Times-BoldItalic"
set output "wifi-manager-example-Arf-802.11b.eps"
set title "Results for 802.11b with Arf\n"
set xlabel "SNR (dB)"
set ylabel "Rate (Mb/s)"
set xrange [-6:15]
set yrange [0:15]
set key top left
plot "-"  title "802.11b-rate selected" with lines, "-"  title "802.11b-observed" with lines
11 11
10 11
9 11
8 11
7 11
6 5.5
5 5.5
4 5.5
3 2
2 2
1 2
0 1
-1 1
-2 1
-3 1
-4 1
-5 1
e
11 4.66125
10 5.27565
9 5.28384
8 5.24288
7 4.44826
6 3.1703
5 3.21946
4 2.74432
3 1.56467
2 1.50733
1 0.991232
0 0.827392
-1 0.827392
-2 0.827392
-3 0.786432
-4 0.32768
-5 0
e
