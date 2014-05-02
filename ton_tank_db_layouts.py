

boat_layout = """

0.0     Telemetrie_Versie       DINT    # #0 Versie van PlC Telemetrie voor AnyWi
4.0     Laag_Batterij           REAL    # 0.000000e+000 Batterij nivo meest lege batterij
8.0     Gem_batterij            REAL    # 0.000000e+000 Gemiddelde van alle baterij nivo's
12.0    Batterij_Alarm          DINT    # L#0 Aantal Tags met batterij Alarm
16.0    Tags_in_Alarm           DINT    # L#0 Aantal Tags in Alarm
20.0    T_Act_V_Mot_1           DINT    # L#0 Secondenteller Voortstuwings Motor 1 Actief
24.0    T_Act_V_Mot_2           DINT    # L#0 Secondenteller Voortstuwings Motor 2 Actief
28.0    T_Act_V_Mot_3           DINT    # L#0 Secondenteller
32.0    T_Act_Gen_MK_Achter     DINT    # L#0 Secondenteller
36.0    T_Act_Gen_MK_Voor       DINT    # L#0 Secondenteller
40.0    T_Act_Boegschroef   `   DINT    # L#0 Secondenteller
44.0    T_Act_Ladingspomp       DINT    # L#0 Secondenteller
48.0    T_Act_Pompmotor         DINT    # L#0 Secondenteller
52.0    T_Act_Schoonmaakpomp    DINT    # L#0 Secondenteller
56.0    T_Act_Ketel_Stage_1     DINT    # L#0 Secondenteller
60.0    T_Act_Ketel_Stage_2     DINT    # L#0 Secondenteller
64.0    T_Act_Ketel_Stage_3     DINT    # L#0 Secondenteller
68.0    Brandstof_V_motor_1     REAL    # 0.000000e+000
72.0    Brandstof_V_motor_2     REAL    # 0.000000e+000
76.0    Brandstof_Totaal        REAL    # 0.000000e+000
80.0    Toerental_Schroef_1     REAL    # 0.000000e+000
84.0    Toerental_Schroef_2     REAL    # 0.000000e+000
88.0    Toerental_Schroef_3     REAL    # 0.000000e+000
92.0    Druk_Vlak_Voorschip     REAL    # 0.000000e+000
96.0    Druk_Vlak_Achterschip   REAL    # 0.000000e+000
100.0   DB10_Actief             DINT    # L#0
"""
