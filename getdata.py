# Iterates over data from https://en.wikipedia.org/wiki/List_of_nearest_galaxies and collects ra, dec and light years.
# Outputs data to csv.


import requests
from bs4 import BeautifulSoup
import pandas as pd

# Make request to first page
rootUrl = "https://en.wikipedia.org/wiki/List_of_nearest_galaxies"
response = requests.get(rootUrl)
soup = BeautifulSoup(response.content, 'html.parser')

table = soup.find('table', {'class': 'wikitable sortable'})

# To hold data
galaxyLst = []
raLst = []
decLst = []
distancesLst = []

# Iterate over galaxy list
for row in table.find_all('tr')[1:]: 
    cols = row.find_all('td')
    if len(cols) < 3:
        continue

    galaxy = cols[2].text.strip()
    print ("\n\n\n\n")
    print (galaxy)

    # Make request to second page
    secondUrl = cols[2].find('a')
    if secondUrl == None:
        continue
    secondUrl = "https://en.wikipedia.org/" + secondUrl['href']
    print (secondUrl)

    response2 = requests.get(secondUrl)
    soup2 = BeautifulSoup(response2.content, 'html.parser')
    raValue = 'N/A'
    decValue = 'N/A'
    distanceValue = 'N/A'

    # Parse ra, dec and distance
    ra = soup2.find(text='Right ascension')
    if ra:
        raValue = ra.find_next('td').text.strip().split("[")[0]

    dec = soup2.find(text='Declination')
    if dec:
        decValue = dec.find_next('td').text.strip().split("[")[0]

    distance = soup2.find(text='Distance')
    if distance:
        distanceValue = distance.find_next('td').text.strip().split("[")[0]

    # Update
    galaxyLst.append(galaxy)
    raLst.append(raValue)
    decLst.append(decValue)
    distancesLst.append(distanceValue)

    print(f"Right Ascension: {raValue}")
    print(f"Declination: {decValue}")
    print(f"Distance: {distanceValue}")


# Save data to csv
data = {
    'Galaxy': galaxyLst,
    'Right Ascension': raLst,
    'Declination': decLst,
    'Distance (light-years)': distancesLst
}
df = pd.DataFrame(data)
df.to_csv('galaxies_equatorial.csv', index=False)
