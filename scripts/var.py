class locations():
    rclone = '/home/pi/.config/rclone/rclone.conf'
    samba = '/etc/samba/smb.conf'
    script_log = '/data/logs/rpi_out.txt'
    script_csv = '/data/logs/rpi_out.csv'
    backups_csv = '/data/pihost/backups.csv'
    pc_mount = '/windata'
    pc_data = '/windata/Hoodi'

class gpin:
    fan = 18
    shutdown = 3
    pc_power = 23
    pc_reset = 24
    pc_status = 25

    top = 0
    tl = 1
    tr = 2
    mid = 3
    bl = 4
    br = 5
    bot = 6

    first = 7
    second = 8
    third = 9