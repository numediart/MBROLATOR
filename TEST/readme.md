Audio files ( the .d16 in the example below) respect LITTLE ENDIAN
format that is PC architecture.

Format of label.txt, each fields separated with spaces

```
file name
|        diphone name
|       |        first sample
|       |        |         last sample
v       |        |        |      middle
        V        V        V      v
1.d16	_ a	2000	3910	2929
2.d16	a k	2000	4027	2939
3.d16	i _	2000	4654	3228
4.d16	k i	2000	3882	3150
```

The phoneme names can be as long as you want and contain non alphabetic
chars such as { as found in the SAMPA alphabet. You may wish to call the
d16 files with something else than numbers... WARNING: DONT INCLUDE NON
ALPHABETIC CHARS IN THE FILE NAME OR UPERCASE LOWERCASE DISTINCTION
(if you work with Windows we loose the upper/lower disctinction and it's
a lot of wasted time).

