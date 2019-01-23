f80=[0 5150 5250 104000]/104000
amp=[1 1 .001 .001]
filtre=remez(512,f80,amp)
showamp(filtre,1)
save filtr16_10 filtre /ascii
