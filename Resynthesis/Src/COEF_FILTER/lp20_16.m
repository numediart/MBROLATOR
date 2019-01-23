f80=[0 7900 8100 40000]/40000
amp=[1 1 .001 .001]
filtre=remez(512,f80,amp)
freqz(filtre,1,512)
save filtr20_16 filtre /ascii
