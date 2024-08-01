# What the 2 GPU kernels do
1) parse_convert_to_cartesian -- parses equatorial coordinate data out of string (scraped from wikipedia using getdata.py) and converts it to equatorial coordinates in radians. Then converts from equatorial to cartesian coordinate system. 
2) calculate_distances -- calculates the distance in the cartesian coordinate system.

# Install
1) Create command line xcode project
2) Add Metal and Foundation to your build
3) Download metal-cpp -- https://github.com/bkaradzic/metal-cpp  
4) add metal-cpp/ to header search path

# Data Collection  
Python script with nested requests to scrape data from each galaxy linked on this list https://en.wikipedia.org/wiki/List_of_nearest_galaxies  

# Project Precursor
CUDA implementation -- https://github.com/Tyler-Hilbert/SpaceTripPlanner

# Example Output
Original data:  
Milky Way 17h 45m 40.03599s −29° 00′ 28.1699″ 26.438  
Canis Major Dwarf 07h 12m 35.0s −27° 40′ 00″ 25  
Segue 1 10h 07m 04s 16° 04′ 55″ 75  
Sagittarius Dwarf Sphr SagDEG 18h 55m 19.5s −30° 32′ 43″ 65  
Ursa Major II Dwarf 08h 51m 30.0s +63° 07′ 48″ 98  
Triangulum II 02h 13m 17.4s +36° 10′ 42.4″ 97.8  
Reticulum II 03h 35m 42.14s −54° 2′ 57.1″ 103  
Segue 2 02h 19m 16s +20° 10′ 31″ 114  
Willman 1 10h 49m 22.3s +51° 03′ 03.6″ 124  
Boötes II 13h 58m 00s +12° 51′ 00″ 136  
Coma Berenices Dwarf 12h 26m 59s +23° 55′ 09″ 143  
Boötes III 13h 57m 00s +26° 48′ 00″ 150  
Large Magellanic Cloud (LMC) 05h 23m 34s −69° 45.4′ 163  
Boötes I 14h 00m 06s +14° 30′ 00″ ± 15″ 197  
Small Magellanic Cloud (SMC NGC 292) 00h 52m 44.8s −72° 49′ 43″ 203.7  
Ursa Minor Dwarf 15h 09m 08.5s +67° 13′ 21″ 200  
Draco Dwarf (DDO 208) 17h 20m 12.4s +57° 54′ 55″ 260  
Pisces Overdensity 23h 19m 00s +00° 00′ 00″ 260  
Sextans Dwarf Sph 10h 13m 02.9s −01° 36′ 53″ 290  
Sculptor Dwarf (ESO 351-G30) 01h 00m 09.3s −33° 42′ 33″ 290  
Virgo I 12h 00m 09.6s −00° 40′ 48″ 280  
Ursa Major I Dwarf (UMa I dSph) 10h 34m 52.8s +51° 55′ 12″ 330  
Carina Dwarf (ESO 206-G220) 06h 41m 36.7s −50° 57′ 58″ 330  
Crater II 11h 49m 14.400s177.310°±0.03° −18° 24′ 46.80″−18.413°±0.03° 383  
Antlia 2 9h 35m 32.832s -36° -46m -2.28s 405  
Hercules Dwarf 16h 31m 02s +12° 47′ 30″ 479  
Fornax Dwarf (ESO 356-G04) 02h 39m 59.3s −34° 26′ 57″ 466  
Canes Venatici II Dwarf 12h 57m 10s +34° 19′ 15″ 490  
Leo IV Dwarf 11h 32m 57s −00° 32′ 00″ 520  
Leo V Dwarf 11h 31m 9.6s +02° 13′ 12″ 585  
Pisces II 22h 58m 31s +05° 57′ 09″ 585  
Leo II Dwarf (Leo B DDO 93) 11h 13m 29.2s +22° 09′ 17″ 690  
Canes Venatici I Dwarf 13h 28m 03.5s +33° 33′ 21″ 711  
Leo I Dwarf (DDO 74 UGC 5470) 10h 08m 27.4s +12° 18′ 27″ 820  
Eridanus II 03h 44m 20.1s (Crnojević et al. 2016) −43° 32′ 01.7″ (Crnojević et al. 2016) 1207  
Leo T Dwarf 09h 34m 53.4s +17° 03′ 05″ 1365  
Phoenix Dwarf Galaxy (P 6830) 01h 51m 06.3s −44° 26′ 41″ 1440  
Barnard's Galaxy (NGC 6822) 19h 44m 56.6s −14° 47′ 21″ 1630  
NGC 185 00h 38m 57.970s +48° 20′ 14.56″ 2050  
Andromeda II 01h 16m 29.8s +33° 25′ 09″ 2220  
IC 1613 (UGC 668) 01h 04m 47.8s +02° 07′ 04″ 2380  
Leo A (Leo III DDO 69) 09h 59m 26.4s +30° 44′ 47″ 2600  
Andromeda XI 00h 46.3m +33° 48′ 05″ 2600  
IC 10 (UGC 192) 00h 20m 17.3s +59° 18′ 14″ 2500  
Andromeda III 00h 35m 33.8s +36° 29′ 52″ 2440  
Cassiopeia Dwarf (Cas dSph Andromeda VII) 23h 26m 31.0s +50° 41′ 31″ 2450  
Cetus Dwarf 00h 26m 11.0s −11° 02′ 40″ 2460  
M32 (NGC 221) 00h 42m 41.8s +40° 51′ 55″ 2490  
Andromeda IX 00h 52m 53.0s +43° 11′ 45″ 2500  
Pisces Dwarf (LGS 3) 01h 03m 55.0s +21° 53′ 06″ 2510  
Andromeda V 01h 10m 17.1s +47° 37′ 41″ 2520  
NGC 147 (DDO 3) 00h 33m 12.1s +48° 30′ 32″ 2530  
Andromeda Galaxy (M31) 00h 42m 44.3s +41° 16′ 9″ 2500  
Pegasus Dwarf Spheroidal (Andromeda VI) 23h 51m 46.3s +24° 34′ 57″ 2700  
Andromeda I 00h 45m 39.8s +38° 02′ 28″ 2400  
M110 (NGC 205) 00h 40m 22.05446s +41° 41′ 07.4963″ 2690  
Andromeda VIII 00h 42m 06.0s +40° 37′ 00″ 2700  
Triangulum Galaxy (M33) 01h 33m 50.02s +30° 39′ 36.7″ 3200  
Andromeda XXI[64] 23h 54m 47.7s +42° 28′ 15″ 2800  
Tucana Dwarf 22h 41m 49.0s −64° 25′ 12″ 3200  
Pegasus Dwarf Irregular (DDO 216) 23h 28m 36.2s +14° 44′ 35″ 3000  
Wolf-Lundmark-Melotte (WLM DDO 221) 00h 01m 58.1s −15° 27′ 39″ 3040  
Andromeda XIX[67] 00h 19m 32.10s +35° 02′ 37.1″ 2651  
Andromeda XXII[64] 01h 27m 40s +28° 05′ 25″ 3070  
Aquarius Dwarf Galaxy (DDO 210) 20h 46m 51.8s −12° 50′ 53″ 3200  
Sagittarius Dwarf Irregular Galaxy (SagDIG) 19h 29m 59.0s −17° 40′ 41″ 3390  
UGC 4879 (VV124)[69] 09h 16m 02.023s +52° 50′ 42.05″ 4180  
Andromeda XVIII[67] 00h 02m 14.5s +45° 05′ 20″ 4420  
Antlia Dwarf 10h 04m 03.9s −27° 19′ 55″ 1320  
Sextans A (UGCA 205 DDO 75) 10h 11m 00.8s −04° 41′ 34″ 4310  
NGC 3109 10h 03m 06.9s −26° 09′ 34″ 1333  
Sextans B (UGC 5373) 10h 00m 00.1s +05° 19′ 56″ 4440  
Leo P 10h 21m 45.123s +18° 05′ 16.89″ 5284  
IC 5152 22h 02m 41.521s −51° 17′ 47.20″ 5870  
NGC 300 00h 54m 53.5s −37° 41′ 04″ 6070  
KKR 25 16h 13m 47.9s +54° 22′ 16″ 6370  
NGC 55 00h 14m 53.6s −39° 11′ 48″ 6500  
UGC 9128 (DDO 187) 14h 15m 56.70s 23° 03′ 16.2″ 7300  
GR 8 (DDO 155) 12h 58m 40.4s +14° 13′ 03″ 7900  
IC 4662 (ESO 102-14) 17h 47.8m −64° 38′ 7960  
UGC 8508 (I Zw 060) 13h 30m 44.4s +54° 54′ 36″ 8500  
Dwingeloo 1 02h 56m 51.9s +58° 54′ 42″ 10000  
UGC 9240 (DDO 190) 14h 24m 43.08s +44° 31′ 31.8″ 9915  
UGCA 133 (DDO 44) 07h 34m 11.5s +66° 52′ 47″ 9820  
NGC 4163 (NGC 4167) 12h 12.9m 36° 10′ 9650  
UGCA 86 03h 59m 50.5s +67° 08′ 37″ 9720  
NGC 1560 04h 32m 49.09s +71° 52′ 59.2″ 9750  
Dwingeloo 2 02h 54m 08.47s +59° 00′ 19.1″ 10000  
MB 3 02h 55m 42.7s "+58° 51' 37""" 10000  
Maffei 2 02h 41m 55.1s +59° 36′ 15″ 9800  
Donatiello I 01h 11m 40.37s +34° 36′ 3.2″ 9700  
NGC 2403 07h 36m 51.4s +65° 36′ 09″ 9650  
NGC 404 01h 09m 27.0s +35° 43′ 04″ 10000  
KKH 22 03h 46m 48.5s +68° 05′ 46″ 10700  
NGC 3741 11h 36m 05s +45° 17′ 02″ 10000  
NGC 2366 7h 28m 54.6s +69° 12′ 57″ 10000  
NGC 1569 (UGC 3056) 4h 30m 49.1s +64° 50′ 53″ 10960  
Sculptor dIG (ESO 349–31) 00h 08m 13.36s −34° 34′ 42.0″ 10440  
IC 342 03h 46m 48.5s +68° 05′ 46″ 10700  
Holmberg II (DDO 50 UGC 4305) 08h 19m 04.98s +70° 43′ 12.1″ 11060  
NGC 5102 13h 21m 57.6s −36° 37′ 49″ 12100  
ESO 540-030 (KDG 2) 00h 49m 21.1s −18° 04′ 34″ 11350  
NGC 247 00h 47m 08.5s −20° 45′ 37″ 11100  
Sculptor Galaxy (NGC 253) 00h 47m 33s −25° 17′ 18″ 11400  
Messier 82 09h 55m 52.2s +69° 40′ 47″ 11900  
NGC 5253 13h 39m 55.9631s −31° 38′ 24.388″ 10900  
NGC 2976 09h 47m 15.458s +67° 54′ 58.97″ 11600  
NGC 4945 13h 05m 27.5s −49° 28′ 06″ 11700  
NGC 6789 19h 16m 41s +63° 58′ 23″ 12000  
Messier 81 09h 55m 33.2s +69° 3′ 55″ 11800  
Holmberg IX (UGC 5336 DDO 66) 09h 57m 32.1s +69° 02′ 46″ 12000  
Centaurus A (NGC 5128) 13h 25m 27.6s −43° 01′ 09″ 12000  
  
  
Kernel Path YOUR_PATH/GPU-Metal-DistanceCalc/GPU-Metal-DistanceCalc/parse_convert_to_cartesian.metal  
  
XYZ  
-5 -21 12  
-5 21 11  
-62 36 20  
0 -55 33  
-22 38 87  
67 39 57  
42 42 83  
92 53 39  
-67 38 96  
-127 -34 30  
-130 0 57  
-129 -35 67  
-33 -127 96  
-165 -95 49  
59 0 193  
-54 -54 184  
-35 -133 220  
251 -66 0  
-251 144 8  
233 62 160  
-279 0 3  
-176 101 259  
0 207 256  
-351 92 120  
-233 233 -233  
-232 -405 106  
332 193 263  
-404 -1 276  
-502 133 4  
-564 150 22  
505 -288 60  
-617 164 260  
-572 -154 393  
-694 400 174  
616 620 831  
-925 920 400  
992 269 1008  
412 -1520 416  
1362 3 1531  
1789 481 1222  
2297 616 87  
-1586 1573 1329  
2160 7 1446  
1276 1 2149  
1961 4 1451  
1499 -398 1895  
2414 4 471  
1883 5 1629  
1822 6 1711  
2249 603 935  
1640 440 1861  
1676 4 1895  
1879 5 1648  
2373 -626 1123  
1890 6 1478  
2008 5 1788  
2049 6 1757  
2657 718 1631  
1997 -526 1890  
1198 -687 2886  
2803 -745 763  
2929 0 810  
2170 2 1522  
2614 706 1445  
1568 -2696 711  
842 -3118 1029  
-1787 1783 3331  
3120 0 3130  
-1015 586 606  
-3721 2144 352  
-1036 598 587  
-3828 2210 412  
-4353 2504 1640  
3178 -1834 4580  
4803 18 3710  
-1852 -3215 5177  
5037 5 4107  
-5813 -3364 2858  
-7657 -32 1940  
-871 -3296 7192  
-4717 -1274 6955  
4461 2599 8563  
-6115 -3545 6952  
-1007 3722 9031  
-7789 -6 5696  
2658 2681 8956  
1509 2629 9266  
4449 2592 8572  
4468 2603 8559  
4286 2492 8452  
7710 2072 5508  
-1041 3847 8788  
7840 2106 5837  
2813 2831 9927  
-6800 1803 7106  
-925 3425 9349  
2320 4039 9920  
8595 5 5925  
2813 2831 9927  
-1830 3160 10439  
-9375 -2527 7219  
10789 38 3521  
10379 35 3934  
10307 35 4869  
-2933 2910 11159  
-8956 -2427 5717  
-3094 3073 10748  
-7343 -1970 8892  
1368 -5084 10783  
-2993 2969 11021  
-3047 3021 11206  
-8470 -2286 8186  
  
  
Kernel Path YOUR_PATH/GPU-Metal-DistanceCalc/GPU-Metal-DistanceCalc/calculate_distances.metal  

Distances from Segue 1:  
Milky Way:	 81 kilo light years  
Canis Major Dwarf:	 59 kilo light years  
Segue 1:	 0 kilo light years  
Sagittarius Dwarf Sphr SagDEG:	 110 kilo light years  
Ursa Major II Dwarf:	 78 kilo light years  
Triangulum II:	 134 kilo light years  
Reticulum II:	 121 kilo light years  
Segue 2:	 156 kilo light years  
Willman 1:	 76 kilo light years  
Boötes II:	 96 kilo light years  
Coma Berenices Dwarf:	 85 kilo light years  
Boötes III:	 108 kilo light years  
Large Magellanic Cloud (LMC):	 182 kilo light years  
Boötes I:	 169 kilo light years  
Small Magellanic Cloud (SMC NGC 292):	 214 kilo light years  
Ursa Minor Dwarf:	 187 kilo light years  
Draco Dwarf (DDO 208):	 263 kilo light years  
Pisces Overdensity:	 329 kilo light years  
Sextans Dwarf Sph:	 218 kilo light years  
Sculptor Dwarf (ESO 351-G30):	 327 kilo light years  
Virgo I:	 220 kilo light years  
Ursa Major I Dwarf (UMa I dSph):	 272 kilo light years  
Carina Dwarf (ESO 206-G220):	 297 kilo light years  
Crater II:	 310 kilo light years  
Antlia 2:	 363 kilo light years  
Hercules Dwarf:	 480 kilo light years  
Fornax Dwarf (ESO 356-G04):	 488 kilo light years  
Canes Venatici II Dwarf:	 428 kilo light years  
Leo IV Dwarf:	 450 kilo light years  
Leo V Dwarf:	 514 kilo light years  
Pisces II:	 654 kilo light years  
Leo II Dwarf (Leo B DDO 93):	 618 kilo light years  
Canes Venatici I Dwarf:	 659 kilo light years  
Leo I Dwarf (DDO 74 UGC 5470):	 745 kilo light years  
Eridanus II:	 1207 kilo light years  
Leo T Dwarf:	 1292 kilo light years  
Phoenix Dwarf Galaxy (P 6830):	 1463 kilo light years  
Barnard's Galaxy (NGC 6822):	 1674 kilo light years  
NGC 185:	 2076 kilo light years  
Andromeda II:	 2251 kilo light years  
IC 1613 (UGC 668):	 2430 kilo light years  
Leo A (Leo III DDO 69):	 2529 kilo light years  
Andromeda XI:	 2640 kilo light years  
IC 10 (UGC 192):	 2514 kilo light years  
Andromeda III:	 2478 kilo light years  
Cassiopeia Dwarf (Cas dSph Andromeda VII):	 2478 kilo light years  
Cetus Dwarf:	 2516 kilo light years  
M32 (NGC 221):	 2524 kilo light years  
Andromeda IX:	 2531 kilo light years  
Pisces Dwarf (LGS 3):	 2549 kilo light years  
Andromeda V:	 2539 kilo light years  
NGC 147 (DDO 3):	 2556 kilo light years  
Andromeda Galaxy (M31):	 2533 kilo light years  
Pegasus Dwarf Spheroidal (Andromeda VI):	 2753 kilo light years  
Andromeda I:	 2436 kilo light years  
M110 (NGC 205):	 2722 kilo light years  
Andromeda VIII:	 2733 kilo light years  
Triangulum Galaxy (M33):	 3233 kilo light years  
Andromeda XXI[64]:	 2837 kilo light years  
Tucana Dwarf:	 3213 kilo light years  
Pegasus Dwarf Irregular (DDO 216):	 3061 kilo light years  
Wolf-Lundmark-Melotte (WLM DDO 221):	 3093 kilo light years  
Andromeda XIX[67]:	 2690 kilo light years  
Andromeda XXII[64]:	 3104 kilo light years  
Aquarius Dwarf Galaxy (DDO 210):	 3255 kilo light years  
Sagittarius Dwarf Irregular Galaxy (SagDIG):	 3432 kilo light years  
UGC 4879 (VV124)[69]:	 4121 kilo light years  
Andromeda XVIII[67]:	 4449 kilo light years  
Antlia Dwarf:	 1246 kilo light years  
Sextans A (UGCA 205 DDO 75):	 4235 kilo light years  
NGC 3109:	 1259 kilo light years  
Sextans B (UGC 5373):	 4366 kilo light years  
Leo P:	 5208 kilo light years  
IC 5152:	 5898 kilo light years  
NGC 300:	 6106 kilo light years  
KKR 25:	 6353 kilo light years  
NGC 55:	 6534 kilo light years  
UGC 9128 (DDO 187):	 7258 kilo light years  
GR 8 (DDO 155):	 7834 kilo light years  
IC 4662 (ESO 102-14):	 7949 kilo light years  
UGC 8508 (I Zw 060):	 8454 kilo light years  
Dwingeloo 1:	 10000 kilo light years  
UGC 9240 (DDO 190):	 9874 kilo light years  
UGCA 133 (DDO 44):	 9781 kilo light years  
NGC 4163 (NGC 4167):	 9587 kilo light years  
UGCA 86:	 9708 kilo light years  
NGC 1560:	 9730 kilo light years  
Dwingeloo 2:	 10000 kilo light years  
MB 3:	 10001 kilo light years  
Maffei 2:	 9799 kilo light years  
Donatiello I:	 9729 kilo light years  
NGC 2403:	 9610 kilo light years  
NGC 404:	 10028 kilo light years  
KKH 22:	 10687 kilo light years  
NGC 3741:	 9936 kilo light years  
NGC 2366:	 9962 kilo light years  
NGC 1569 (UGC 3056):	 10941 kilo light years  
Sculptor dIG (ESO 349–31):	 10479 kilo light years  
IC 342:	 10687 kilo light years  
Holmberg II (DDO 50 UGC 4305):	 11020 kilo light years  
NGC 5102:	 12046 kilo light years  
ESO 540-030 (KDG 2):	 11401 kilo light years  
NGC 247:	 11150 kilo light years  
Sculptor Galaxy (NGC 253):	 11446 kilo light years  
Messier 82:	 11856 kilo light years  
NGC 5253:	 10845 kilo light years  
NGC 2976:	 11554 kilo light years  
NGC 4945:	 11651 kilo light years  
NGC 6789:	 12004 kilo light years  
Messier 81:	 11756 kilo light years  
Holmberg IX (UGC 5336 DDO 66):	 11956 kilo light years  
Centaurus A (NGC 5128):	 11948 kilo light years  
