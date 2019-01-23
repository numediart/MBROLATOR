test : diph_resynth
	database_build test RES/ RES/ 2.069 16000 120 1
RES/0.f0 : WAV/1.d16 test.f0
	anaf0 'WAV/1.d16' RES/0.f0 2000 3810 test.f0
RES/0.mbe: RES/0.f0 test.mbe
	anambe WAV/1.d16 RES/0.f0 RES/0.mbe 2000 3810 test.mbe
RES/0.s16 : RES/0.mbe test.syn
	 resynth WAV/1.d16 RES/0.mbe RES/0 test.syn

RES/1.f0 : WAV/2.d16 test.f0
	anaf0 'WAV/2.d16' RES/1.f0 2000 3927 test.f0
RES/1.mbe: RES/1.f0 test.mbe
	anambe WAV/2.d16 RES/1.f0 RES/1.mbe 2000 3927 test.mbe
RES/1.s16 : RES/1.mbe test.syn
	 resynth WAV/2.d16 RES/1.mbe RES/1 test.syn

RES/2.f0 : WAV/3.d16 test.f0
	anaf0 'WAV/3.d16' RES/2.f0 2000 4554 test.f0
RES/2.mbe: RES/2.f0 test.mbe
	anambe WAV/3.d16 RES/2.f0 RES/2.mbe 2000 4554 test.mbe
RES/2.s16 : RES/2.mbe test.syn
	 resynth WAV/3.d16 RES/2.mbe RES/2 test.syn

RES/3.f0 : WAV/4.d16 test.f0
	anaf0 'WAV/4.d16' RES/3.f0 2000 3782 test.f0
RES/3.mbe: RES/3.f0 test.mbe
	anambe WAV/4.d16 RES/3.f0 RES/3.mbe 2000 3782 test.mbe
RES/3.s16 : RES/3.mbe test.syn
	 resynth WAV/4.d16 RES/3.mbe RES/3 test.syn

diph_resynth: RES/0.s16 RES/1.s16 RES/2.s16 RES/3.s16 
