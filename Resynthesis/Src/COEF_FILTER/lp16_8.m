f80=[0 7900 8100 16000]/16000
amp=[1 1 .0001 .001]
filtre=remez(200,f80,amp)
freqz(filtre,1)
zoom on
save filtr16_8 filtre /ascii
