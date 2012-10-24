set xtics 0.01 rotate by -90
set grid

set style line 1 lt rgb "red" lw 1
set style line 2 lt rgb "green" lw 1
set style line 3 lt rgb "blue" lw 1
set style line 4 lt rgb "cyan" lw 1
set style line 5 lt rgb "magenta" lw 1
set style line 6 lt rgb "yellow" lw 1
set style line 7 lt rgb "black" lw 1

plot "goertz_ch0" using 1:2 ls 1 title "700" with lines,\
     "goertz_ch0" using 1:3 ls 2 title "900" with lines,\
     "goertz_ch0" using 1:4 ls 3 title "1100" with lines,\
     "goertz_ch0" using 1:5 ls 4 title "1300" with lines,\
     "goertz_ch0" using 1:6 ls 5 title "1500" with lines,\
     "goertz_ch0" using 1:7 ls 6 title "1700" with lines,\
     "goertz_ch0" using 1:8 ls 7 title "wave" with lines
