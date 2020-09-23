EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:Battery_Cell BT?
U 1 1 5F6E2D20
P 1650 2800
AR Path="/5F6E2D20" Ref="BT?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D20" Ref="BT?"  Part="1" 
F 0 "BT?" H 1768 2896 50  0000 L CNN
F 1 "Battery_Cell" H 1768 2805 50  0000 L CNN
F 2 "" V 1650 2860 50  0001 C CNN
F 3 "~" V 1650 2860 50  0001 C CNN
	1    1650 2800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5F6E2D26
P 1650 2900
AR Path="/5F6E2D26" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D26" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 1650 2650 50  0001 C CNN
F 1 "GND" H 1655 2727 50  0000 C CNN
F 2 "" H 1650 2900 50  0001 C CNN
F 3 "" H 1650 2900 50  0001 C CNN
	1    1650 2900
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:IRLML6402 Q?
U 1 1 5F6E2D2C
P 2500 2700
AR Path="/5F6E2D2C" Ref="Q?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D2C" Ref="Q?"  Part="1" 
F 0 "Q?" V 2842 2700 50  0000 C CNN
F 1 "IRLML6402" V 2751 2700 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2700 2625 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml6402pbf.pdf?fileId=5546d462533600a401535668d5c2263c" H 2500 2700 50  0001 L CNN
	1    2500 2700
	0    -1   -1   0   
$EndComp
$Comp
L Transistor_FET:IRLML6402 Q?
U 1 1 5F6E2D32
P 3050 2700
AR Path="/5F6E2D32" Ref="Q?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D32" Ref="Q?"  Part="1" 
F 0 "Q?" V 3392 2700 50  0000 C CNN
F 1 "IRLML6402" V 3301 2700 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3250 2625 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml6402pbf.pdf?fileId=5546d462533600a401535668d5c2263c" H 3050 2700 50  0001 L CNN
	1    3050 2700
	0    1    -1   0   
$EndComp
Wire Wire Line
	1650 2600 2200 2600
Wire Wire Line
	2500 2900 3050 2900
Wire Wire Line
	2700 2600 2850 2600
$Comp
L Device:R R?
U 1 1 5F6E2D3B
P 2200 2750
AR Path="/5F6E2D3B" Ref="R?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D3B" Ref="R?"  Part="1" 
F 0 "R?" H 2270 2796 50  0000 L CNN
F 1 "200k " H 2270 2705 50  0000 L CNN
F 2 "" V 2130 2750 50  0001 C CNN
F 3 "~" H 2200 2750 50  0001 C CNN
	1    2200 2750
	1    0    0    -1  
$EndComp
Connection ~ 2200 2600
Wire Wire Line
	2200 2600 2300 2600
Wire Wire Line
	2500 2900 2200 2900
Connection ~ 2500 2900
Wire Wire Line
	3050 2900 3500 2900
Wire Wire Line
	3500 2900 3500 1950
Wire Wire Line
	3500 1950 3550 1950
Connection ~ 3050 2900
$Comp
L Diode:BAT54C D?
U 1 1 5F6E2D49
P 3750 1950
AR Path="/5F6E2D49" Ref="D?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D49" Ref="D?"  Part="1" 
F 0 "D?" V 3704 2038 50  0000 L CNN
F 1 "BAT54C" V 3795 2038 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3825 2075 50  0001 L CNN
F 3 "http://www.diodes.com/_files/datasheets/ds11005.pdf" H 3670 1950 50  0001 C CNN
	1    3750 1950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5F6E2D4F
P 3750 2250
AR Path="/5F6E2D4F" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D4F" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3750 2000 50  0001 C CNN
F 1 "GND" H 3755 2077 50  0000 C CNN
F 2 "" H 3750 2250 50  0001 C CNN
F 3 "" H 3750 2250 50  0001 C CNN
	1    3750 2250
	1    0    0    -1  
$EndComp
$Comp
L power:+12C #PWR?
U 1 1 5F6E2D55
P 3750 1650
AR Path="/5F6E2D55" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D55" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 3750 1500 50  0001 C CNN
F 1 "+12C" H 3765 1823 50  0000 C CNN
F 2 "" H 3750 1650 50  0001 C CNN
F 3 "" H 3750 1650 50  0001 C CNN
	1    3750 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5F6E2D5B
P 3050 3050
AR Path="/5F6E2D5B" Ref="R?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D5B" Ref="R?"  Part="1" 
F 0 "R?" H 3120 3096 50  0000 L CNN
F 1 "10k " H 3120 3005 50  0000 L CNN
F 2 "" V 2980 3050 50  0001 C CNN
F 3 "~" H 3050 3050 50  0001 C CNN
	1    3050 3050
	1    0    0    -1  
$EndComp
$Comp
L Diode:BAT54C D?
U 1 1 5F6E2D61
P 4500 1950
AR Path="/5F6E2D61" Ref="D?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D61" Ref="D?"  Part="1" 
F 0 "D?" V 4454 2038 50  0000 L CNN
F 1 "BAT54C" V 4545 2038 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4575 2075 50  0001 L CNN
F 3 "http://www.diodes.com/_files/datasheets/ds11005.pdf" H 4420 1950 50  0001 C CNN
	1    4500 1950
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5F6E2D67
P 4500 2250
AR Path="/5F6E2D67" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D67" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4500 2000 50  0001 C CNN
F 1 "GND" H 4505 2077 50  0000 C CNN
F 2 "" H 4500 2250 50  0001 C CNN
F 3 "" H 4500 2250 50  0001 C CNN
	1    4500 2250
	1    0    0    -1  
$EndComp
$Comp
L power:+12C #PWR?
U 1 1 5F6E2D6D
P 4500 1650
AR Path="/5F6E2D6D" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D6D" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4500 1500 50  0001 C CNN
F 1 "+12C" H 4515 1823 50  0000 C CNN
F 2 "" H 4500 1650 50  0001 C CNN
F 3 "" H 4500 1650 50  0001 C CNN
	1    4500 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 2600 4250 2600
Wire Wire Line
	4700 1950 4700 2600
Connection ~ 4700 2600
$Comp
L Device:R R?
U 1 1 5F6E2D76
P 4250 2750
AR Path="/5F6E2D76" Ref="R?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D76" Ref="R?"  Part="1" 
F 0 "R?" H 4320 2796 50  0000 L CNN
F 1 "10k " H 4320 2705 50  0000 L CNN
F 2 "" V 4180 2750 50  0001 C CNN
F 3 "~" H 4250 2750 50  0001 C CNN
	1    4250 2750
	1    0    0    -1  
$EndComp
Connection ~ 4250 2600
Wire Wire Line
	4250 2600 4700 2600
$Comp
L Device:R R?
U 1 1 5F6E2D7E
P 4250 3050
AR Path="/5F6E2D7E" Ref="R?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D7E" Ref="R?"  Part="1" 
F 0 "R?" H 4320 3096 50  0000 L CNN
F 1 "10k " H 4320 3005 50  0000 L CNN
F 2 "" V 4180 3050 50  0001 C CNN
F 3 "~" H 4250 3050 50  0001 C CNN
	1    4250 3050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5F6E2D84
P 4250 3200
AR Path="/5F6E2D84" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D84" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4250 2950 50  0001 C CNN
F 1 "GND" H 4255 3027 50  0000 C CNN
F 2 "" H 4250 3200 50  0001 C CNN
F 3 "" H 4250 3200 50  0001 C CNN
	1    4250 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5F6E2D8A
P 4600 3100
AR Path="/5F6E2D8A" Ref="C?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D8A" Ref="C?"  Part="1" 
F 0 "C?" H 4715 3146 50  0000 L CNN
F 1 "100n" H 4715 3055 50  0000 L CNN
F 2 "" H 4638 2950 50  0001 C CNN
F 3 "~" H 4600 3100 50  0001 C CNN
	1    4600 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 2900 4600 2900
Wire Wire Line
	4600 2900 4600 2950
Connection ~ 4250 2900
$Comp
L power:GND #PWR?
U 1 1 5F6E2D93
P 4600 3250
AR Path="/5F6E2D93" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D93" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4600 3000 50  0001 C CNN
F 1 "GND" H 4605 3077 50  0000 C CNN
F 2 "" H 4600 3250 50  0001 C CNN
F 3 "" H 4600 3250 50  0001 C CNN
	1    4600 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 2900 4900 2900
Connection ~ 4600 2900
Wire Wire Line
	4700 2600 5450 2600
Wire Wire Line
	3050 3600 2650 3600
Wire Wire Line
	3050 3200 3050 3600
$Comp
L Transistor_FET:BSS123 Q?
U 1 1 5F6E2D9E
P 2550 3800
AR Path="/5F6E2D9E" Ref="Q?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2D9E" Ref="Q?"  Part="1" 
F 0 "Q?" H 2754 3846 50  0000 L CNN
F 1 "BSS123" H 2754 3755 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2750 3725 50  0001 L CIN
F 3 "http://www.diodes.com/assets/Datasheets/ds30366.pdf" H 2550 3800 50  0001 L CNN
	1    2550 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 3800 2150 3800
$Comp
L power:GND #PWR?
U 1 1 5F6E2DA5
P 2650 4000
AR Path="/5F6E2DA5" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2DA5" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2650 3750 50  0001 C CNN
F 1 "GND" H 2655 3827 50  0000 C CNN
F 2 "" H 2650 4000 50  0001 C CNN
F 3 "" H 2650 4000 50  0001 C CNN
	1    2650 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 4650 2700 4650
$Comp
L Device:CircuitBreaker_1P CB?
U 1 1 5F6E2DAE
P 2350 4650
AR Path="/5F6E2DAE" Ref="CB?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2DAE" Ref="CB?"  Part="1" 
F 0 "CB?" V 2085 4650 50  0000 C CNN
F 1 "CircuitBreaker_1P" V 2176 4650 50  0000 C CNN
F 2 "" H 2350 4650 50  0001 C CNN
F 3 "~" H 2350 4650 50  0001 C CNN
	1    2350 4650
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5F6E2DB4
P 2700 4800
AR Path="/5F6E2DB4" Ref="C?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2DB4" Ref="C?"  Part="1" 
F 0 "C?" H 2815 4846 50  0000 L CNN
F 1 "100n" H 2815 4755 50  0000 L CNN
F 2 "" H 2738 4650 50  0001 C CNN
F 3 "~" H 2700 4800 50  0001 C CNN
	1    2700 4800
	1    0    0    -1  
$EndComp
Connection ~ 2700 4650
Wire Wire Line
	2700 4650 2650 4650
$Comp
L power:GND #PWR?
U 1 1 5F6E2DBC
P 2700 4950
AR Path="/5F6E2DBC" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2DBC" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2700 4700 50  0001 C CNN
F 1 "GND" H 2705 4777 50  0000 C CNN
F 2 "" H 2700 4950 50  0001 C CNN
F 3 "" H 2700 4950 50  0001 C CNN
	1    2700 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5F6E2DC2
P 2050 4650
AR Path="/5F6E2DC2" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2DC2" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2050 4400 50  0001 C CNN
F 1 "GND" H 2055 4477 50  0000 C CNN
F 2 "" H 2050 4650 50  0001 C CNN
F 3 "" H 2050 4650 50  0001 C CNN
	1    2050 4650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5F6E2DC8
P 2150 3950
AR Path="/5F6E2DC8" Ref="R?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2DC8" Ref="R?"  Part="1" 
F 0 "R?" H 2220 3996 50  0000 L CNN
F 1 "10k " H 2220 3905 50  0000 L CNN
F 2 "" V 2080 3950 50  0001 C CNN
F 3 "~" H 2150 3950 50  0001 C CNN
	1    2150 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 3800 1800 3800
Connection ~ 2150 3800
$Comp
L power:GND #PWR?
U 1 1 5F6E2DD0
P 2150 4100
AR Path="/5F6E2DD0" Ref="#PWR?"  Part="1" 
AR Path="/5F6C9CC0/5F6E2DD0" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2150 3850 50  0001 C CNN
F 1 "GND" H 2155 3927 50  0000 C CNN
F 2 "" H 2150 4100 50  0001 C CNN
F 3 "" H 2150 4100 50  0001 C CNN
	1    2150 4100
	1    0    0    -1  
$EndComp
Text GLabel 5450 2600 2    50   Input ~ 0
Vo
Text GLabel 1800 3800 0    50   Input ~ 0
постоянка_мк
Text GLabel 4900 2900 2    50   Input ~ 0
тип_питания
Connection ~ 3050 3600
Wire Wire Line
	3050 3600 3050 4650
$EndSCHEMATC
