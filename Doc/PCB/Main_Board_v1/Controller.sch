EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
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
L Connector_Generic:Conn_01x02 J?
U 1 1 5EDF6AAA
P 950 1100
AR Path="/5EDF6AAA" Ref="J?"  Part="1" 
AR Path="/5EA602EB/5EDF6AAA" Ref="J?"  Part="1" 
AR Path="/5EA60E89/5EDF6AAA" Ref="J?"  Part="1" 
AR Path="/5E9339D0/5EDF6AAA" Ref="J7"  Part="1" 
F 0 "J7" H 868 775 50  0000 C CNN
F 1 "Conn_01x02" H 868 866 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Horizontal" H 950 1100 50  0001 C CNN
F 3 "~" H 950 1100 50  0001 C CNN
	1    950  1100
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6AB0
P 1300 1300
AR Path="/5EDF6AB0" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6AB0" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6AB0" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6AB0" Ref="#PWR038"  Part="1" 
F 0 "#PWR038" H 1300 1050 50  0001 C CNN
F 1 "GND" H 1305 1127 50  0000 C CNN
F 2 "" H 1300 1300 50  0001 C CNN
F 3 "" H 1300 1300 50  0001 C CNN
	1    1300 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 1100 1300 1100
Wire Wire Line
	1300 1100 1300 1300
$Comp
L Power_Management:7805 U?
U 1 1 5EDF6AB8
P 1950 1400
AR Path="/5EDF6AB8" Ref="U?"  Part="1" 
AR Path="/5EA602EB/5EDF6AB8" Ref="U?"  Part="1" 
AR Path="/5EA60E89/5EDF6AB8" Ref="U?"  Part="1" 
AR Path="/5E9339D0/5EDF6AB8" Ref="U4"  Part="1" 
F 0 "U4" H 2175 1765 50  0000 C CNN
F 1 "7805" H 2175 1674 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 1900 1650 50  0001 C CNN
F 3 "" H 1900 1650 50  0001 C CNN
	1    1950 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 1000 1650 1000
Wire Wire Line
	1650 1000 1650 1350
Wire Wire Line
	1650 1350 1850 1350
$Comp
L power:GND #PWR?
U 1 1 5EDF6AC1
P 2150 1800
AR Path="/5EDF6AC1" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6AC1" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6AC1" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6AC1" Ref="#PWR040"  Part="1" 
F 0 "#PWR040" H 2150 1550 50  0001 C CNN
F 1 "GND" H 2155 1627 50  0000 C CNN
F 2 "" H 2150 1800 50  0001 C CNN
F 3 "" H 2150 1800 50  0001 C CNN
	1    2150 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 1800 2150 1650
Wire Wire Line
	1650 1000 3050 1000
Connection ~ 1650 1000
Wire Wire Line
	2500 1350 2700 1350
$Comp
L Device:C C?
U 1 1 5EDF6ACB
P 1650 1600
AR Path="/5EDF6ACB" Ref="C?"  Part="1" 
AR Path="/5EA602EB/5EDF6ACB" Ref="C?"  Part="1" 
AR Path="/5EA60E89/5EDF6ACB" Ref="C?"  Part="1" 
AR Path="/5E9339D0/5EDF6ACB" Ref="C12"  Part="1" 
F 0 "C12" H 1765 1646 50  0000 L CNN
F 1 "4.7uF 50V" H 1765 1555 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L9.0mm_W3.3mm_P7.50mm_MKT" H 1688 1450 50  0001 C CNN
F 3 "~" H 1650 1600 50  0001 C CNN
	1    1650 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5EDF6AD1
P 2700 1600
AR Path="/5EDF6AD1" Ref="C?"  Part="1" 
AR Path="/5EA602EB/5EDF6AD1" Ref="C?"  Part="1" 
AR Path="/5EA60E89/5EDF6AD1" Ref="C?"  Part="1" 
AR Path="/5E9339D0/5EDF6AD1" Ref="C13"  Part="1" 
F 0 "C13" H 2815 1646 50  0000 L CNN
F 1 "100nF" H 2815 1555 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L9.0mm_W3.3mm_P7.50mm_MKT" H 2738 1450 50  0001 C CNN
F 3 "~" H 2700 1600 50  0001 C CNN
	1    2700 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6AD7
P 2700 1800
AR Path="/5EDF6AD7" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6AD7" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6AD7" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6AD7" Ref="#PWR045"  Part="1" 
F 0 "#PWR045" H 2700 1550 50  0001 C CNN
F 1 "GND" H 2705 1627 50  0000 C CNN
F 2 "" H 2700 1800 50  0001 C CNN
F 3 "" H 2700 1800 50  0001 C CNN
	1    2700 1800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6ADD
P 1650 1800
AR Path="/5EDF6ADD" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6ADD" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6ADD" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6ADD" Ref="#PWR039"  Part="1" 
F 0 "#PWR039" H 1650 1550 50  0001 C CNN
F 1 "GND" H 1655 1627 50  0000 C CNN
F 2 "" H 1650 1800 50  0001 C CNN
F 3 "" H 1650 1800 50  0001 C CNN
	1    1650 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 1750 1650 1800
Wire Wire Line
	2700 1750 2700 1800
Wire Wire Line
	2700 1450 2700 1350
Connection ~ 2700 1350
Wire Wire Line
	2700 1350 3050 1350
Connection ~ 1650 1350
Wire Wire Line
	1650 1350 1650 1450
$Comp
L power:+5V #PWR?
U 1 1 5EDF6AEA
P 3050 1250
AR Path="/5EDF6AEA" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6AEA" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6AEA" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6AEA" Ref="#PWR047"  Part="1" 
F 0 "#PWR047" H 3050 1100 50  0001 C CNN
F 1 "+5V" H 3065 1423 50  0000 C CNN
F 2 "" H 3050 1250 50  0001 C CNN
F 3 "" H 3050 1250 50  0001 C CNN
	1    3050 1250
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5EDF6AF0
P 3050 850
AR Path="/5EDF6AF0" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6AF0" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6AF0" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6AF0" Ref="#PWR046"  Part="1" 
F 0 "#PWR046" H 3050 700 50  0001 C CNN
F 1 "+12V" H 3065 1023 50  0000 C CNN
F 2 "" H 3050 850 50  0001 C CNN
F 3 "" H 3050 850 50  0001 C CNN
	1    3050 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 850  3050 1000
Wire Wire Line
	3050 1250 3050 1350
$Comp
L power:GND #PWR?
U 1 1 5EDF6B18
P 4600 4150
AR Path="/5EDF6B18" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B18" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B18" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B18" Ref="#PWR056"  Part="1" 
F 0 "#PWR056" H 4600 3900 50  0001 C CNN
F 1 "GND" H 4605 3977 50  0000 C CNN
F 2 "" H 4600 4150 50  0001 C CNN
F 3 "" H 4600 4150 50  0001 C CNN
	1    4600 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6B1E
P 6950 4150
AR Path="/5EDF6B1E" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B1E" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B1E" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B1E" Ref="#PWR059"  Part="1" 
F 0 "#PWR059" H 6950 3900 50  0001 C CNN
F 1 "GND" H 6955 3977 50  0000 C CNN
F 2 "" H 6950 4150 50  0001 C CNN
F 3 "" H 6950 4150 50  0001 C CNN
	1    6950 4150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5EDF6B24
P 6950 1500
AR Path="/5EDF6B24" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B24" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B24" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B24" Ref="#PWR058"  Part="1" 
F 0 "#PWR058" H 6950 1350 50  0001 C CNN
F 1 "+5V" H 6965 1673 50  0000 C CNN
F 2 "" H 6950 1500 50  0001 C CNN
F 3 "" H 6950 1500 50  0001 C CNN
	1    6950 1500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J?
U 1 1 5EDF6B2A
P 9650 4650
AR Path="/5EDF6B2A" Ref="J?"  Part="1" 
AR Path="/5EA602EB/5EDF6B2A" Ref="J?"  Part="1" 
AR Path="/5EA60E89/5EDF6B2A" Ref="J?"  Part="1" 
AR Path="/5E9339D0/5EDF6B2A" Ref="J12"  Part="1" 
F 0 "J12" H 9622 4624 50  0000 R CNN
F 1 "BUZZER" H 9622 4533 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Horizontal" H 9650 4650 50  0001 C CNN
F 3 "~" H 9650 4650 50  0001 C CNN
	1    9650 4650
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Male J?
U 1 1 5EDF6B30
P 2900 2300
AR Path="/5EDF6B30" Ref="J?"  Part="1" 
AR Path="/5EA602EB/5EDF6B30" Ref="J?"  Part="1" 
AR Path="/5EA60E89/5EDF6B30" Ref="J?"  Part="1" 
AR Path="/5E9339D0/5EDF6B30" Ref="J10"  Part="1" 
F 0 "J10" H 3008 2581 50  0000 C CNN
F 1 "PTR1" H 3008 2490 50  0000 C CNN
F 2 "" H 2900 2300 50  0001 C CNN
F 3 "~" H 2900 2300 50  0001 C CNN
	1    2900 2300
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5EDF6B36
P 3200 2050
AR Path="/5EDF6B36" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B36" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B36" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B36" Ref="#PWR048"  Part="1" 
F 0 "#PWR048" H 3200 1900 50  0001 C CNN
F 1 "+5V" H 3215 2223 50  0000 C CNN
F 2 "" H 3200 2050 50  0001 C CNN
F 3 "" H 3200 2050 50  0001 C CNN
	1    3200 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6B3C
P 3200 2500
AR Path="/5EDF6B3C" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B3C" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B3C" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B3C" Ref="#PWR049"  Part="1" 
F 0 "#PWR049" H 3200 2250 50  0001 C CNN
F 1 "GND" H 3205 2327 50  0000 C CNN
F 2 "" H 3200 2500 50  0001 C CNN
F 3 "" H 3200 2500 50  0001 C CNN
	1    3200 2500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6B42
P 3850 2750
AR Path="/5EDF6B42" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B42" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B42" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B42" Ref="#PWR052"  Part="1" 
F 0 "#PWR052" H 3850 2500 50  0001 C CNN
F 1 "GND" H 3855 2577 50  0000 C CNN
F 2 "" H 3850 2750 50  0001 C CNN
F 3 "" H 3850 2750 50  0001 C CNN
	1    3850 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 2200 3200 2200
Wire Wire Line
	3200 2200 3200 2050
Wire Wire Line
	3100 2400 3200 2400
Wire Wire Line
	3200 2400 3200 2500
$Comp
L Device:C C?
U 1 1 5EDF6B4C
P 3850 2500
AR Path="/5EDF6B4C" Ref="C?"  Part="1" 
AR Path="/5EA602EB/5EDF6B4C" Ref="C?"  Part="1" 
AR Path="/5EA60E89/5EDF6B4C" Ref="C?"  Part="1" 
AR Path="/5E9339D0/5EDF6B4C" Ref="C16"  Part="1" 
F 0 "C16" H 3965 2546 50  0000 L CNN
F 1 "1uF" H 3965 2455 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W2.5mm_P5.00mm" H 3888 2350 50  0001 C CNN
F 3 "~" H 3850 2500 50  0001 C CNN
	1    3850 2500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6B52
P 3500 2750
AR Path="/5EDF6B52" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B52" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B52" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B52" Ref="#PWR051"  Part="1" 
F 0 "#PWR051" H 3500 2500 50  0001 C CNN
F 1 "GND" H 3505 2577 50  0000 C CNN
F 2 "" H 3500 2750 50  0001 C CNN
F 3 "" H 3500 2750 50  0001 C CNN
	1    3500 2750
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5EDF6B58
P 3500 2500
AR Path="/5EDF6B58" Ref="C?"  Part="1" 
AR Path="/5EA602EB/5EDF6B58" Ref="C?"  Part="1" 
AR Path="/5EA60E89/5EDF6B58" Ref="C?"  Part="1" 
AR Path="/5E9339D0/5EDF6B58" Ref="C15"  Part="1" 
F 0 "C15" H 3615 2546 50  0000 L CNN
F 1 "1uF" H 3615 2455 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W2.5mm_P5.00mm" H 3538 2350 50  0001 C CNN
F 3 "~" H 3500 2500 50  0001 C CNN
	1    3500 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 2650 3500 2750
Wire Wire Line
	3850 2650 3850 2750
Wire Wire Line
	3200 2200 3500 2200
Wire Wire Line
	3500 2200 3500 2350
Connection ~ 3200 2200
Wire Wire Line
	3100 2300 3850 2300
Wire Wire Line
	3850 2300 3850 2350
$Comp
L power:GND #PWR?
U 1 1 5EDF6B65
P 9400 4850
AR Path="/5EDF6B65" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B65" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B65" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B65" Ref="#PWR063"  Part="1" 
F 0 "#PWR063" H 9400 4600 50  0001 C CNN
F 1 "GND" H 9405 4677 50  0000 C CNN
F 2 "" H 9400 4850 50  0001 C CNN
F 3 "" H 9400 4850 50  0001 C CNN
	1    9400 4850
	-1   0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5EDF6B6B
P 4450 1000
AR Path="/5EDF6B6B" Ref="C?"  Part="1" 
AR Path="/5EA602EB/5EDF6B6B" Ref="C?"  Part="1" 
AR Path="/5EA60E89/5EDF6B6B" Ref="C?"  Part="1" 
AR Path="/5E9339D0/5EDF6B6B" Ref="C17"  Part="1" 
F 0 "C17" H 4565 1046 50  0000 L CNN
F 1 "100nF" H 4565 955 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L9.0mm_W3.3mm_P7.50mm_MKT" H 4488 850 50  0001 C CNN
F 3 "~" H 4450 1000 50  0001 C CNN
	1    4450 1000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6B71
P 4450 1250
AR Path="/5EDF6B71" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B71" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B71" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B71" Ref="#PWR054"  Part="1" 
F 0 "#PWR054" H 4450 1000 50  0001 C CNN
F 1 "GND" H 4455 1077 50  0000 C CNN
F 2 "" H 4450 1250 50  0001 C CNN
F 3 "" H 4450 1250 50  0001 C CNN
	1    4450 1250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5EDF6B77
P 4450 750
AR Path="/5EDF6B77" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B77" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B77" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B77" Ref="#PWR053"  Part="1" 
F 0 "#PWR053" H 4450 600 50  0001 C CNN
F 1 "+5V" H 4465 923 50  0000 C CNN
F 2 "" H 4450 750 50  0001 C CNN
F 3 "" H 4450 750 50  0001 C CNN
	1    4450 750 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 750  4450 850 
Wire Wire Line
	4450 1150 4450 1250
$Comp
L Device:C C?
U 1 1 5EDF6B7F
P 3400 1250
AR Path="/5EDF6B7F" Ref="C?"  Part="1" 
AR Path="/5EA602EB/5EDF6B7F" Ref="C?"  Part="1" 
AR Path="/5EA60E89/5EDF6B7F" Ref="C?"  Part="1" 
AR Path="/5E9339D0/5EDF6B7F" Ref="C14"  Part="1" 
F 0 "C14" H 3515 1296 50  0000 L CNN
F 1 "100nF" H 3515 1205 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L9.0mm_W3.3mm_P7.50mm_MKT" H 3438 1100 50  0001 C CNN
F 3 "~" H 3400 1250 50  0001 C CNN
	1    3400 1250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6B85
P 3400 1500
AR Path="/5EDF6B85" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B85" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B85" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B85" Ref="#PWR050"  Part="1" 
F 0 "#PWR050" H 3400 1250 50  0001 C CNN
F 1 "GND" H 3405 1327 50  0000 C CNN
F 2 "" H 3400 1500 50  0001 C CNN
F 3 "" H 3400 1500 50  0001 C CNN
	1    3400 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 1500 3400 1400
Wire Wire Line
	3400 1100 3400 1000
Wire Wire Line
	3400 1000 3050 1000
Connection ~ 3050 1000
$Comp
L LCD:LCD U?
U 1 1 5EDF6B8F
P 7450 1250
AR Path="/5EDF6B8F" Ref="U?"  Part="1" 
AR Path="/5EA602EB/5EDF6B8F" Ref="U?"  Part="1" 
AR Path="/5EA60E89/5EDF6B8F" Ref="U?"  Part="1" 
AR Path="/5E9339D0/5EDF6B8F" Ref="U6"  Part="1" 
F 0 "U6" H 7750 1465 50  0000 C CNN
F 1 "LCD" H 7750 1374 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 7150 1350 50  0001 C CNN
F 3 "" H 7150 1350 50  0001 C CNN
	1    7450 1250
	1    0    0    -1  
$EndComp
Text GLabel 4150 3550 0    50   BiDi ~ 0
SCL
Text GLabel 6700 3550 2    50   BiDi ~ 0
SDA
Wire Wire Line
	5500 3550 4150 3550
$Comp
L Device:R_POT RV?
U 1 1 5EDF6B98
P 8000 1950
AR Path="/5EDF6B98" Ref="RV?"  Part="1" 
AR Path="/5EA602EB/5EDF6B98" Ref="RV?"  Part="1" 
AR Path="/5EA60E89/5EDF6B98" Ref="RV?"  Part="1" 
AR Path="/5E9339D0/5EDF6B98" Ref="RV1"  Part="1" 
F 0 "RV1" V 7885 1950 50  0000 C CNN
F 1 "5k" V 7794 1950 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 8000 1950 50  0001 C CNN
F 3 "~" H 8000 1950 50  0001 C CNN
	1    8000 1950
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6B9E
P 7800 2000
AR Path="/5EDF6B9E" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6B9E" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6B9E" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6B9E" Ref="#PWR060"  Part="1" 
F 0 "#PWR060" H 7800 1750 50  0001 C CNN
F 1 "GND" H 7805 1827 50  0000 C CNN
F 2 "" H 7800 2000 50  0001 C CNN
F 3 "" H 7800 2000 50  0001 C CNN
	1    7800 2000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5EDF6BA4
P 8200 1800
AR Path="/5EDF6BA4" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6BA4" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6BA4" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6BA4" Ref="#PWR061"  Part="1" 
F 0 "#PWR061" H 8200 1650 50  0001 C CNN
F 1 "+5V" H 8215 1973 50  0000 C CNN
F 2 "" H 8200 1800 50  0001 C CNN
F 3 "" H 8200 1800 50  0001 C CNN
	1    8200 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 1800 8200 1850
Wire Wire Line
	8200 1950 8150 1950
Wire Wire Line
	8100 1850 8200 1850
Connection ~ 8200 1850
Wire Wire Line
	8200 1850 8200 1950
Wire Wire Line
	7900 1400 7900 1500
Wire Wire Line
	7900 1800 7800 1800
Wire Wire Line
	7800 1800 7800 1950
Wire Wire Line
	7850 1950 7800 1950
Connection ~ 7800 1950
Wire Wire Line
	7800 1950 7800 2000
Wire Wire Line
	8200 1400 8200 1500
Wire Wire Line
	8200 1500 7900 1500
Wire Wire Line
	8100 1400 8100 1850
Wire Wire Line
	8000 1400 8000 1800
Wire Wire Line
	7900 1500 7900 1800
Connection ~ 7900 1500
$Comp
L Device:R R?
U 1 1 5EDF6BBB
P 8400 1650
AR Path="/5EDF6BBB" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6BBB" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6BBB" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6BBB" Ref="R14"  Part="1" 
F 0 "R14" H 8470 1696 50  0000 L CNN
F 1 "100" H 8470 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8330 1650 50  0001 C CNN
F 3 "~" H 8400 1650 50  0001 C CNN
	1    8400 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 1400 8300 1450
Wire Wire Line
	8300 1450 8400 1450
Wire Wire Line
	8400 1450 8400 1500
Wire Wire Line
	8400 1800 8400 1850
Wire Wire Line
	8400 1850 8200 1850
Wire Wire Line
	7700 1400 7700 1500
Wire Wire Line
	7700 1500 7900 1500
Wire Wire Line
	7200 2050 7200 1400
Wire Wire Line
	7300 2150 7300 1400
Wire Wire Line
	7400 2250 7400 1400
Wire Wire Line
	7500 2350 7500 1400
Wire Wire Line
	7600 1400 7600 2450
Wire Wire Line
	7800 1400 7800 1600
Wire Wire Line
	7800 1600 7700 1600
Wire Wire Line
	7700 1600 7700 2550
$Comp
L Device:R R?
U 1 1 5EDF6BD0
P 8000 2650
AR Path="/5EDF6BD0" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6BD0" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6BD0" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6BD0" Ref="R13"  Part="1" 
F 0 "R13" V 7793 2650 50  0000 C CNN
F 1 "470" V 7884 2650 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7930 2650 50  0001 C CNN
F 3 "~" H 8000 2650 50  0001 C CNN
	1    8000 2650
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint_2Pole TP?
U 1 1 5EDF6BD6
P 10450 2650
AR Path="/5EDF6BD6" Ref="TP?"  Part="1" 
AR Path="/5EA602EB/5EDF6BD6" Ref="TP?"  Part="1" 
AR Path="/5EA60E89/5EDF6BD6" Ref="TP?"  Part="1" 
AR Path="/5E9339D0/5EDF6BD6" Ref="TP4"  Part="1" 
F 0 "TP4" H 10450 2845 50  0000 C CNN
F 1 "LED Breath" H 10450 2754 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10450 2650 50  0001 C CNN
F 3 "~" H 10450 2650 50  0001 C CNN
	1    10450 2650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EDF6BDC
P 10850 4600
AR Path="/5EDF6BDC" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6BDC" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6BDC" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6BDC" Ref="#PWR064"  Part="1" 
F 0 "#PWR064" H 10850 4350 50  0001 C CNN
F 1 "GND" H 10855 4427 50  0000 C CNN
F 2 "" H 10850 4600 50  0001 C CNN
F 3 "" H 10850 4600 50  0001 C CNN
	1    10850 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10650 2650 10850 2650
$Comp
L Device:R R?
U 1 1 5EDF6BE3
P 3200 3800
AR Path="/5EDF6BE3" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6BE3" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6BE3" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6BE3" Ref="R11"  Part="1" 
F 0 "R11" V 2993 3800 50  0000 C CNN
F 1 "R" V 3084 3800 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3130 3800 50  0001 C CNN
F 3 "~" H 3200 3800 50  0001 C CNN
	1    3200 3800
	0    -1   1    0   
$EndComp
$Comp
L Diode:1N4001 D?
U 1 1 5EDF6BE9
P 2550 3250
AR Path="/5EDF6BE9" Ref="D?"  Part="1" 
AR Path="/5EA602EB/5EDF6BE9" Ref="D?"  Part="1" 
AR Path="/5EA60E89/5EDF6BE9" Ref="D?"  Part="1" 
AR Path="/5E9339D0/5EDF6BE9" Ref="D2"  Part="1" 
F 0 "D2" V 2504 3329 50  0000 L CNN
F 1 "1N4001" V 2595 3329 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 2550 3075 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 2550 3250 50  0001 C CNN
	1    2550 3250
	0    -1   1    0   
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5EDF6BEF
P 2550 2950
AR Path="/5EDF6BEF" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6BEF" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6BEF" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6BEF" Ref="#PWR041"  Part="1" 
F 0 "#PWR041" H 2550 2800 50  0001 C CNN
F 1 "+12V" H 2565 3123 50  0000 C CNN
F 2 "" H 2550 2950 50  0001 C CNN
F 3 "" H 2550 2950 50  0001 C CNN
	1    2550 2950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2550 2950 2550 3050
Wire Wire Line
	3050 3800 2850 3800
$Comp
L power:GND #PWR?
U 1 1 5EDF6BF7
P 2550 4150
AR Path="/5EDF6BF7" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6BF7" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6BF7" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6BF7" Ref="#PWR042"  Part="1" 
F 0 "#PWR042" H 2550 3900 50  0001 C CNN
F 1 "GND" H 2555 3977 50  0000 C CNN
F 2 "" H 2550 4150 50  0001 C CNN
F 3 "" H 2550 4150 50  0001 C CNN
	1    2550 4150
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2550 4150 2550 4000
Wire Wire Line
	2550 3400 2550 3500
Wire Wire Line
	1800 3050 2550 3050
Connection ~ 2550 3050
Wire Wire Line
	2550 3050 2550 3100
Wire Wire Line
	2550 3500 2000 3500
Wire Wire Line
	2000 3500 2000 3150
Wire Wire Line
	2000 3150 1800 3150
Connection ~ 2550 3500
Wire Wire Line
	2550 3500 2550 3600
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EDF6C07
P 1550 4900
AR Path="/5EDF6C07" Ref="J?"  Part="1" 
AR Path="/5EA602EB/5EDF6C07" Ref="J?"  Part="1" 
AR Path="/5EA60E89/5EDF6C07" Ref="J?"  Part="1" 
AR Path="/5E9339D0/5EDF6C07" Ref="J8"  Part="1" 
F 0 "J8" H 1468 5117 50  0000 C CNN
F 1 "SP3" H 1468 5026 50  0000 C CNN
F 2 "" H 1550 4900 50  0001 C CNN
F 3 "~" H 1550 4900 50  0001 C CNN
	1    1550 4900
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EDF6C0D
P 3200 5650
AR Path="/5EDF6C0D" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6C0D" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6C0D" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6C0D" Ref="R12"  Part="1" 
F 0 "R12" V 2993 5650 50  0000 C CNN
F 1 "R" V 3084 5650 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3130 5650 50  0001 C CNN
F 3 "~" H 3200 5650 50  0001 C CNN
	1    3200 5650
	0    -1   1    0   
$EndComp
$Comp
L Diode:1N4001 D?
U 1 1 5EDF6C13
P 2550 5100
AR Path="/5EDF6C13" Ref="D?"  Part="1" 
AR Path="/5EA602EB/5EDF6C13" Ref="D?"  Part="1" 
AR Path="/5EA60E89/5EDF6C13" Ref="D?"  Part="1" 
AR Path="/5E9339D0/5EDF6C13" Ref="D3"  Part="1" 
F 0 "D3" V 2504 5179 50  0000 L CNN
F 1 "1N4001" V 2595 5179 50  0000 L CNN
F 2 "Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal" H 2550 4925 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 2550 5100 50  0001 C CNN
	1    2550 5100
	0    -1   1    0   
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 5EDF6C19
P 2550 4800
AR Path="/5EDF6C19" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6C19" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6C19" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6C19" Ref="#PWR043"  Part="1" 
F 0 "#PWR043" H 2550 4650 50  0001 C CNN
F 1 "+12V" H 2565 4973 50  0000 C CNN
F 2 "" H 2550 4800 50  0001 C CNN
F 3 "" H 2550 4800 50  0001 C CNN
	1    2550 4800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2550 4800 2550 4900
Wire Wire Line
	3050 5650 2850 5650
$Comp
L power:GND #PWR?
U 1 1 5EDF6C21
P 2550 6000
AR Path="/5EDF6C21" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6C21" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6C21" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6C21" Ref="#PWR044"  Part="1" 
F 0 "#PWR044" H 2550 5750 50  0001 C CNN
F 1 "GND" H 2555 5827 50  0000 C CNN
F 2 "" H 2550 6000 50  0001 C CNN
F 3 "" H 2550 6000 50  0001 C CNN
	1    2550 6000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2550 6000 2550 5850
Wire Wire Line
	2550 5250 2550 5350
Wire Wire Line
	1750 4900 2550 4900
Connection ~ 2550 4900
Wire Wire Line
	2550 4900 2550 4950
Wire Wire Line
	2550 5350 2000 5350
Wire Wire Line
	2000 5350 2000 5000
Wire Wire Line
	2000 5000 1750 5000
Connection ~ 2550 5350
Wire Wire Line
	2550 5350 2550 5450
$Comp
L Connector:TestPoint_2Pole TP?
U 1 1 5EDF6C31
P 10450 2950
AR Path="/5EDF6C31" Ref="TP?"  Part="1" 
AR Path="/5EA602EB/5EDF6C31" Ref="TP?"  Part="1" 
AR Path="/5EA60E89/5EDF6C31" Ref="TP?"  Part="1" 
AR Path="/5E9339D0/5EDF6C31" Ref="TP5"  Part="1" 
F 0 "TP5" H 10450 3145 50  0000 C CNN
F 1 "Breath" H 10450 3054 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10450 2950 50  0001 C CNN
F 3 "~" H 10450 2950 50  0001 C CNN
	1    10450 2950
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint_2Pole TP?
U 1 1 5EDF6C37
P 10450 3250
AR Path="/5EDF6C37" Ref="TP?"  Part="1" 
AR Path="/5EA602EB/5EDF6C37" Ref="TP?"  Part="1" 
AR Path="/5EA60E89/5EDF6C37" Ref="TP?"  Part="1" 
AR Path="/5E9339D0/5EDF6C37" Ref="TP6"  Part="1" 
F 0 "TP6" H 10450 3445 50  0000 C CNN
F 1 "+" H 10450 3354 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10450 3250 50  0001 C CNN
F 3 "~" H 10450 3250 50  0001 C CNN
	1    10450 3250
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint_2Pole TP?
U 1 1 5EDF6C3D
P 10450 3550
AR Path="/5EDF6C3D" Ref="TP?"  Part="1" 
AR Path="/5EA602EB/5EDF6C3D" Ref="TP?"  Part="1" 
AR Path="/5EA60E89/5EDF6C3D" Ref="TP?"  Part="1" 
AR Path="/5E9339D0/5EDF6C3D" Ref="TP7"  Part="1" 
F 0 "TP7" H 10450 3745 50  0000 C CNN
F 1 "-" H 10450 3654 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10450 3550 50  0001 C CNN
F 3 "~" H 10450 3550 50  0001 C CNN
	1    10450 3550
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint_2Pole TP?
U 1 1 5EDF6C43
P 10450 3850
AR Path="/5EDF6C43" Ref="TP?"  Part="1" 
AR Path="/5EA602EB/5EDF6C43" Ref="TP?"  Part="1" 
AR Path="/5EA60E89/5EDF6C43" Ref="TP?"  Part="1" 
AR Path="/5E9339D0/5EDF6C43" Ref="TP8"  Part="1" 
F 0 "TP8" H 10450 4045 50  0000 C CNN
F 1 "BPM" H 10450 3954 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10450 3850 50  0001 C CNN
F 3 "~" H 10450 3850 50  0001 C CNN
	1    10450 3850
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint_2Pole TP?
U 1 1 5EDF6C49
P 10450 4150
AR Path="/5EDF6C49" Ref="TP?"  Part="1" 
AR Path="/5EA602EB/5EDF6C49" Ref="TP?"  Part="1" 
AR Path="/5EA60E89/5EDF6C49" Ref="TP?"  Part="1" 
AR Path="/5E9339D0/5EDF6C49" Ref="TP9"  Part="1" 
F 0 "TP9" H 10450 4345 50  0000 C CNN
F 1 "EP" H 10450 4254 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10450 4150 50  0001 C CNN
F 3 "~" H 10450 4150 50  0001 C CNN
	1    10450 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EDF6C4F
P 8800 2400
AR Path="/5EDF6C4F" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6C4F" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6C4F" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6C4F" Ref="R16"  Part="1" 
F 0 "R16" H 8870 2446 50  0000 L CNN
F 1 "10k" H 8870 2355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8730 2400 50  0001 C CNN
F 3 "~" H 8800 2400 50  0001 C CNN
	1    8800 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EDF6C55
P 9100 2400
AR Path="/5EDF6C55" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6C55" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6C55" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6C55" Ref="R17"  Part="1" 
F 0 "R17" H 9170 2446 50  0000 L CNN
F 1 "10k" H 9170 2355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9030 2400 50  0001 C CNN
F 3 "~" H 9100 2400 50  0001 C CNN
	1    9100 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EDF6C5B
P 9400 2400
AR Path="/5EDF6C5B" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6C5B" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6C5B" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6C5B" Ref="R18"  Part="1" 
F 0 "R18" H 9470 2446 50  0000 L CNN
F 1 "10k" H 9470 2355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9330 2400 50  0001 C CNN
F 3 "~" H 9400 2400 50  0001 C CNN
	1    9400 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EDF6C61
P 9700 2400
AR Path="/5EDF6C61" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6C61" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6C61" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6C61" Ref="R19"  Part="1" 
F 0 "R19" H 9770 2446 50  0000 L CNN
F 1 "10k" H 9770 2355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9630 2400 50  0001 C CNN
F 3 "~" H 9700 2400 50  0001 C CNN
	1    9700 2400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5EDF6C67
P 10000 2400
AR Path="/5EDF6C67" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5EDF6C67" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5EDF6C67" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5EDF6C67" Ref="R20"  Part="1" 
F 0 "R20" H 10070 2446 50  0000 L CNN
F 1 "10k" H 10070 2355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9930 2400 50  0001 C CNN
F 3 "~" H 10000 2400 50  0001 C CNN
	1    10000 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 3050 7850 3250
Wire Wire Line
	7850 3250 9700 3250
Wire Wire Line
	7700 3150 7700 3550
Wire Wire Line
	7700 3550 9400 3550
Wire Wire Line
	10850 2650 10850 2950
Wire Wire Line
	10650 4150 10850 4150
Connection ~ 10850 4150
Wire Wire Line
	10650 3850 10850 3850
Connection ~ 10850 3850
Wire Wire Line
	10850 3850 10850 4150
Wire Wire Line
	10650 3550 10850 3550
Connection ~ 10850 3550
Wire Wire Line
	10850 3550 10850 3850
Wire Wire Line
	10650 3250 10850 3250
Connection ~ 10850 3250
Wire Wire Line
	10850 3250 10850 3550
Wire Wire Line
	10650 2950 10850 2950
Connection ~ 10850 2950
Wire Wire Line
	10850 2950 10850 3250
Wire Wire Line
	10000 2550 10000 2950
Connection ~ 10000 2950
Wire Wire Line
	10000 2950 10250 2950
Wire Wire Line
	9700 2550 9700 3250
Connection ~ 9700 3250
Wire Wire Line
	9700 3250 10250 3250
Wire Wire Line
	9400 2550 9400 3550
Connection ~ 9400 3550
Wire Wire Line
	9400 3550 10250 3550
Wire Wire Line
	9100 2550 9100 3850
Connection ~ 9100 3850
Wire Wire Line
	9100 3850 10250 3850
Wire Wire Line
	10250 4150 8800 4150
$Comp
L power:+5V #PWR?
U 1 1 5EDF6C98
P 9400 1900
AR Path="/5EDF6C98" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6C98" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6C98" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6C98" Ref="#PWR062"  Part="1" 
F 0 "#PWR062" H 9400 1750 50  0001 C CNN
F 1 "+5V" H 9415 2073 50  0000 C CNN
F 2 "" H 9400 1900 50  0001 C CNN
F 3 "" H 9400 1900 50  0001 C CNN
	1    9400 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 2250 10000 2050
Wire Wire Line
	8800 2550 8800 4150
Connection ~ 8800 4150
Wire Wire Line
	3350 3800 3500 3800
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5EDF6CBC
P 1600 3050
AR Path="/5EDF6CBC" Ref="J?"  Part="1" 
AR Path="/5EA602EB/5EDF6CBC" Ref="J?"  Part="1" 
AR Path="/5EA60E89/5EDF6CBC" Ref="J?"  Part="1" 
AR Path="/5E9339D0/5EDF6CBC" Ref="J9"  Part="1" 
F 0 "J9" H 1518 3267 50  0000 C CNN
F 1 "SP2" H 1518 3176 50  0000 C CNN
F 2 "" H 1600 3050 50  0001 C CNN
F 3 "~" H 1600 3050 50  0001 C CNN
	1    1600 3050
	-1   0    0    -1  
$EndComp
$Comp
L Transistor_FET:IRLZ34N Q?
U 1 1 5EDF6CC2
P 2650 3800
AR Path="/5EDF6CC2" Ref="Q?"  Part="1" 
AR Path="/5EA602EB/5EDF6CC2" Ref="Q?"  Part="1" 
AR Path="/5EA60E89/5EDF6CC2" Ref="Q?"  Part="1" 
AR Path="/5E9339D0/5EDF6CC2" Ref="Q2"  Part="1" 
F 0 "Q2" H 2854 3846 50  0000 L CNN
F 1 "IRLZ34N" H 2854 3755 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 2900 3725 50  0001 L CIN
F 3 "http://www.infineon.com/dgdl/irlz34npbf.pdf?fileId=5546d462533600a40153567206892720" H 2650 3800 50  0001 L CNN
	1    2650 3800
	-1   0    0    -1  
$EndComp
$Comp
L Transistor_FET:IRLZ34N Q?
U 1 1 5EDF6CC8
P 2650 5650
AR Path="/5EDF6CC8" Ref="Q?"  Part="1" 
AR Path="/5EA602EB/5EDF6CC8" Ref="Q?"  Part="1" 
AR Path="/5EA60E89/5EDF6CC8" Ref="Q?"  Part="1" 
AR Path="/5E9339D0/5EDF6CC8" Ref="Q3"  Part="1" 
F 0 "Q3" H 2854 5696 50  0000 L CNN
F 1 "IRLZ34N" H 2854 5605 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 2900 5575 50  0001 L CIN
F 3 "http://www.infineon.com/dgdl/irlz34npbf.pdf?fileId=5546d462533600a40153567206892720" H 2650 5650 50  0001 L CNN
	1    2650 5650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6650 2050 7200 2050
Wire Wire Line
	6650 2150 7300 2150
Wire Wire Line
	6650 2250 7400 2250
Wire Wire Line
	6650 2350 7500 2350
Wire Wire Line
	6650 2450 7600 2450
Wire Wire Line
	6650 2850 7700 2850
Wire Wire Line
	7700 2850 7700 2650
Wire Wire Line
	7700 2650 7850 2650
Wire Wire Line
	6650 2950 10000 2950
Wire Wire Line
	6650 3050 7850 3050
Wire Wire Line
	6650 3150 7700 3150
$Comp
L MCU_Microchip_PIC18:XPRESS-40 U?
U 1 1 5EDF6CD9
P 6400 2200
AR Path="/5EA602EB/5EDF6CD9" Ref="U?"  Part="1" 
AR Path="/5EA60E89/5EDF6CD9" Ref="U?"  Part="1" 
AR Path="/5E9339D0/5EDF6CD9" Ref="U5"  Part="1" 
F 0 "U5" H 6075 3315 50  0000 C CNN
F 1 "XPRESS-40" H 6075 3224 50  0000 C CNN
F 2 "" H 6400 2200 50  0001 C CNN
F 3 "" H 6400 2200 50  0001 C CNN
	1    6400 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 2550 7700 2550
Wire Wire Line
	7850 3650 7850 3850
Wire Wire Line
	7850 3850 9100 3850
Wire Wire Line
	7700 4150 7700 3750
Wire Wire Line
	7700 4150 8800 4150
$Comp
L power:+5V #PWR?
U 1 1 5EDF6CE4
P 4600 1400
AR Path="/5EDF6CE4" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5EDF6CE4" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5EDF6CE4" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5EDF6CE4" Ref="#PWR055"  Part="1" 
F 0 "#PWR055" H 4600 1250 50  0001 C CNN
F 1 "+5V" H 4615 1573 50  0000 C CNN
F 2 "" H 4600 1400 50  0001 C CNN
F 3 "" H 4600 1400 50  0001 C CNN
	1    4600 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 1400 4600 2850
Wire Wire Line
	6650 2650 6950 2650
Wire Wire Line
	6950 1500 6950 2650
Wire Wire Line
	6950 2750 6950 4150
Wire Wire Line
	4600 2950 4600 4150
Wire Wire Line
	9400 4850 9400 4750
Wire Wire Line
	9400 4750 9450 4750
Wire Wire Line
	3850 2300 3850 1950
Wire Wire Line
	3850 1950 5500 1950
Connection ~ 3850 2300
NoConn ~ 6650 3250
NoConn ~ 6650 3350
NoConn ~ 6650 3450
NoConn ~ 6650 1850
NoConn ~ 6650 1950
NoConn ~ 5500 1850
NoConn ~ 5500 2050
NoConn ~ 5500 2150
NoConn ~ 5500 2350
NoConn ~ 5500 2450
NoConn ~ 5500 2550
NoConn ~ 5500 2650
NoConn ~ 5500 2750
NoConn ~ 5500 3450
NoConn ~ 5500 3650
NoConn ~ 5500 3750
Wire Wire Line
	6650 3550 6700 3550
Wire Wire Line
	6650 3650 7850 3650
Wire Wire Line
	7700 3750 6650 3750
Wire Wire Line
	6650 2750 6950 2750
Wire Wire Line
	5500 2050 4150 2050
Wire Wire Line
	4150 2050 4150 3100
Wire Wire Line
	4150 3100 3500 3100
Wire Wire Line
	3500 3100 3500 3800
Wire Wire Line
	5500 2150 4400 2150
Wire Wire Line
	4400 2150 4400 5650
Wire Wire Line
	4400 5650 3350 5650
Wire Wire Line
	5500 3650 5200 3650
Wire Wire Line
	5200 3650 5200 4650
Wire Wire Line
	5200 4650 9450 4650
Wire Wire Line
	4600 2850 5500 2850
Wire Wire Line
	4600 2950 5500 2950
$Comp
L Connector:TestPoint_2Pole TP?
U 1 1 5F06AC85
P 10450 4450
AR Path="/5F06AC85" Ref="TP?"  Part="1" 
AR Path="/5EA602EB/5F06AC85" Ref="TP?"  Part="1" 
AR Path="/5EA60E89/5F06AC85" Ref="TP?"  Part="1" 
AR Path="/5E9339D0/5F06AC85" Ref="TP10"  Part="1" 
F 0 "TP10" H 10450 4645 50  0000 C CNN
F 1 "IP" H 10450 4554 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 10450 4450 50  0001 C CNN
F 3 "~" H 10450 4450 50  0001 C CNN
	1    10450 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	10850 4150 10850 4450
Wire Wire Line
	10650 4450 10850 4450
Connection ~ 10850 4450
Wire Wire Line
	10850 4450 10850 4600
$Comp
L Device:R R?
U 1 1 5F0AE860
P 8500 2400
AR Path="/5F0AE860" Ref="R?"  Part="1" 
AR Path="/5EA602EB/5F0AE860" Ref="R?"  Part="1" 
AR Path="/5EA60E89/5F0AE860" Ref="R?"  Part="1" 
AR Path="/5E9339D0/5F0AE860" Ref="R15"  Part="1" 
F 0 "R15" H 8570 2446 50  0000 L CNN
F 1 "10k" H 8570 2355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8430 2400 50  0001 C CNN
F 3 "~" H 8500 2400 50  0001 C CNN
	1    8500 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 2250 8500 2050
Wire Wire Line
	8500 2550 8500 4450
Wire Wire Line
	8500 4450 10250 4450
$Comp
L Connector:Conn_01x03_Male J11
U 1 1 5F0CC73F
P 5500 5400
F 0 "J11" H 5472 5424 50  0000 R CNN
F 1 "UART" H 5472 5333 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Horizontal" H 5500 5400 50  0001 C CNN
F 3 "~" H 5500 5400 50  0001 C CNN
	1    5500 5400
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5500 3750 5350 3750
Wire Wire Line
	5350 3750 5350 4450
Wire Wire Line
	5350 4450 8500 4450
Connection ~ 8500 4450
Wire Wire Line
	4950 5500 5300 5500
Wire Wire Line
	5050 5300 5300 5300
$Comp
L power:GND #PWR?
U 1 1 5F126EC5
P 5200 5650
AR Path="/5F126EC5" Ref="#PWR?"  Part="1" 
AR Path="/5EA602EB/5F126EC5" Ref="#PWR?"  Part="1" 
AR Path="/5EA60E89/5F126EC5" Ref="#PWR?"  Part="1" 
AR Path="/5E9339D0/5F126EC5" Ref="#PWR057"  Part="1" 
F 0 "#PWR057" H 5200 5400 50  0001 C CNN
F 1 "GND" H 5205 5477 50  0000 C CNN
F 2 "" H 5200 5650 50  0001 C CNN
F 3 "" H 5200 5650 50  0001 C CNN
	1    5200 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 5400 5200 5400
Wire Wire Line
	5200 5400 5200 5650
Wire Wire Line
	8150 2650 10250 2650
Wire Wire Line
	8500 2050 8800 2050
Wire Wire Line
	9700 2250 9700 2050
Connection ~ 9700 2050
Wire Wire Line
	9700 2050 10000 2050
Wire Wire Line
	9400 2250 9400 2050
Connection ~ 9400 2050
Wire Wire Line
	9400 2050 9700 2050
Wire Wire Line
	9100 2250 9100 2050
Connection ~ 9100 2050
Wire Wire Line
	9100 2050 9400 2050
Wire Wire Line
	8800 2250 8800 2050
Connection ~ 8800 2050
Wire Wire Line
	8800 2050 9100 2050
Wire Wire Line
	9400 1900 9400 2050
Wire Wire Line
	5500 3250 4950 3250
Wire Wire Line
	4950 3250 4950 5500
Wire Wire Line
	5500 3350 5050 3350
Wire Wire Line
	5050 3350 5050 5300
$EndSCHEMATC