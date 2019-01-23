f80=[0 4750 5250 40000]/40000
amp=[1 1 .001 .001]
filtre=remez(300,f80,amp)
showamp(filtre,1)
save filtr16_10 filtre /ascii
