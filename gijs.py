import time
import logging
import libnodave
from libnodave.utils import tofloat, tobigint


logging.basicConfig(level=logging.DEBUG)

link = libnodave.Libnodave()
link.open_socket('10.0.0.10', 1099)
link.new_interface('if1', 0, 223, 2)
link.set_timeout(50000000)
link.init_adapter()
link.connect_plc(2, 0, 2)


def actie(address, type_):
    value = link.read_bytes(area=0x84, db=10, start=address, length=4)
    return type_(value)

data = (
    (0, "Telemetrie_Versie", tobigint),
    (4, "Laag_Batterij", tofloat),
    (8, "Gem_batterij", tofloat),
    (12, "Batterij_Alarm",tobigint),
    (16, "Tags_in_Alarm", tobigint),
    (20, "T_Act_V_Mot_1", tobigint),
    (24, "T_Act_V_Mot_2", tobigint),
    (28, "T_Act_V_Mot_3", tobigint),
    (32, "T_Act_Gen_MK_Achter", tobigint),
    (36, "T_Act_Gen_MK_Voor", tobigint),
    (40, "T_Act_Boegschroef", tobigint),
    (44, "T_Act_Ladingspomp", tobigint),
    (48, "T_Act_Pompmotor", tobigint),
    (52, "T_Act_Schoonmaakpomp", tobigint),
    (56, "T_Act_Ketel_Stage_1", tobigint),
    (60, "T_Act_Ketel_Stage_2", tobigint),
    (64, "T_Act_Ketel_Stage_3", tobigint),
    (68, "Brandstof_V_motor_1", tofloat),
    (72, "Brandstof_V_motor_2", tofloat),
    (76, "Brandstof_Totaal",tofloat),
    (80, "Toerental_Schroef_1", tofloat),
    (84, "Toerental_Schroef_2", tofloat),
    (88, " Toerental_Schroef_3", tofloat),
    (92, "Druk_Vlak_Voorschip", tofloat),
    (96, "Druk_Vlak_Achterschip", tofloat),
    (100, "DB10_Actief", tobigint)
)

for i in range(10):
    for address, name, type_ in data:
        print name + ": " + str(actie(address,  type_))
    time.sleep(1)


link.disconnect()
