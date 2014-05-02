
#import time

import snap7
from ton_tank_db_layouts import boat_layout

client = snap7.client.Client()
client.connect('10.0.0.10', 0, 2)


def make_boat_data():
    boat_data = client.db_upload(10)
    db10 = snap7.util.db.DB(10, boat_data,
                            boat_layout, 104, 1)

    print db10[0]


if __name__ == '__main__':
    make_boat_data()
