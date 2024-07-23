import math
import pandas as pd
import csv

def hms_to_radians(hours, minutes, seconds):
    return (hours + minutes / 60 + seconds / 3600) * (math.pi / 12)

def dms_to_radians(degrees, minutes, seconds):
    sign = 1 if degrees >= 0 else -1
    return sign * (abs(degrees) + minutes / 60 + seconds / 3600) * (math.pi / 180)

def parse_ra_dec(ra, dec):
    try:
        if ra == "N/A" or dec == "N/A":
            return "N/A", "N/A"

        ra_parts = ra.replace("h", " ").replace("m", " ").replace("s", "").split()
        ra_radians = hms_to_radians(float(ra_parts[0]), float(ra_parts[1]), float(ra_parts[2]))

        dec_sign = -1 if "−" in dec or "-" in dec else 1
        dec = dec.replace("−", "-").replace("+", "").replace("°", " ").replace("′", " ").replace("″", "").split()
        if len(dec) == 2:
            dec.append('0')
        dec_radians = dms_to_radians(dec_sign * float(dec[0]), float(dec[1]), float(dec[2]))

        return ra_radians, dec_radians
    except Exception as e:
        print(f"Error parsing RA/Dec for RA: {ra}, Dec: {dec}. Error: {e}")
        return "N/A", "N/A"


galaxies_data = []

with open('galaxies_equatorial_lyprocess.csv', mode ='r') as file:
    csvFile = csv.reader(file)
    next(csvFile)  # Skip the header
    for line in csvFile:
        print(line)
        galaxy = line[0]
        ra = line[1]
        dec = line[2]
        distance = line[3]

        ra_radians, dec_radians = parse_ra_dec(ra, dec)
        galaxies_data.append({
            "Galaxy": galaxy,
            "Right Ascension (radians)": ra_radians,
            "Declination (radians)": dec_radians,
            "Distance (kly)": distance
        })
for galaxy in galaxies_data:
    if "N/A" in str(galaxy["Right Ascension (radians)"]):
        continue
    stringData = "name: " + str(galaxy['Galaxy']) + ", ra: " + str(galaxy["Right Ascension (radians)"]) + ", dec: " + str(galaxy["Declination (radians)"]) + ", lightyears: " + str(galaxy["Distance (kly)"])
    print (stringData)
