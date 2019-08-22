/** 
 * @@ Copyright (The Age of Stone Project)
 *
 * The Age of Stone (TaoS): Version v0.1 Alpha
 * Copyright (c) 2002, by Owen Varley <owen@interactiontraining.com>
 */

//
// Weather.c - Holds all functions and code needed for the Taos weather system.
//

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "music.h"


////////////////////////////////////////////////////////////////////////////////////
// Function: Weather_create
// Usage   : Called to create the weather file (weather.dat) this holds 192 game 
//		   : hours of weather patterns, after 192 game hours a new file is created
//		   : 192 game hours is roughly 4 days.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void weather_create(void)
{
	// File stuff
	FILE *fp;
	char file[MIL];
	// Variables
	int curr_weather = 99;
	int old_weather = SKY_CLOUDLESS;
	int percent = 0;
	int i;

	// Open and create Weather file
	sprintf (file, "%s%s", DATA_DIR, WEATHER_FILE);
	if (!(fp = fopen (file, "w")))
	{
		bugf ("[weather.c::weather_create] fopen of %s failed", file);
		return;
	}

	for (i = 0;i < 192; i++)
	{
    // Make 1d100 roll to determine weather
	percent = random_dice(1, 100, 0);

	switch (time_info.season)
	{
	// Generates weather for the Summer Season
	// Dependent on previous weather and 1d100 roll
	case SEASON_SUMMER:
		if (old_weather == SKY_CLOUDY)
		{
			if (percent <= 30)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 30 && percent <= 60)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 60)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_CLOUDLESS)
		{
			if (percent <= 20)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 20)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_RAINING)
		{
			if (percent <= 50)
			{
				curr_weather = SKY_RAINING;
			fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 50 && percent <= 75)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 75 && percent <= 95)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 95)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_LIGHTNING)
		{
			if (percent <= 40)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 40 && percent <= 50)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 50 && percent <= 90)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 90)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_HAIL)
		{
			if (percent <= 30)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 30 && percent <= 40)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 40 && percent <= 90)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 90)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		// Generates weather for the Spring Season
		// Dependent on previous weather and 1d100 roll
	case SEASON_SPRING:
		if (old_weather == SKY_CLOUDY)
		{
			if (percent <= 30)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 30 && percent <= 60)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 60)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_CLOUDLESS)
		{
			if (percent <= 20)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 20)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_RAINING)
		{
			if (percent <= 50)
			{
				curr_weather = SKY_RAINING;
			fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 50 && percent <= 70)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 70 && percent <= 95)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 95)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_LIGHTNING)
		{
			if (percent <= 50)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 50 && percent <= 55)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 55 && percent <= 80)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 80)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_HAIL)
		{
			if (percent <= 35)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 35 && percent <= 45)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 45 && percent <= 90)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 90)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		// Generates weather for the Autumn Season
		// Dependent on previous weather and 1d100 roll
	case SEASON_AUTUMN:
		if (old_weather == SKY_CLOUDY)
		{
			if (percent <= 20)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 20 && percent <= 40)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 40 && percent <= 95)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 95)
			{
				curr_weather = SKY_SNOW;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_CLOUDLESS)
		{
			if (percent <= 40)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 40)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_RAINING)
		{
			if (percent <= 35)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 35 && percent <= 85)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 85 && percent <= 95)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 95)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_LIGHTNING)
		{
			if (percent <= 20)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 20 && percent <= 40)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 40 && percent <= 95)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 95)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_HAIL)
		{
			if (percent <= 20)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 20 && percent <= 60)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 60 && percent <= 95)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 95)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_SNOW)
		{
			if (percent <= 90)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 90)
			{
				curr_weather = SKY_SNOW;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		// Generates weather for the Winter Season
		// Dependent on previous weather and 1d100 roll
	case SEASON_WINTER:
		if (old_weather == SKY_CLOUDY)
		{
			if (percent <= 10)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 10 && percent <= 25)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 25 && percent <= 75)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 75)
			{
				curr_weather = SKY_SNOW;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_CLOUDLESS)
		{
			if (percent <= 40)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 40)
			{
				curr_weather = SKY_CLOUDLESS;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_RAINING)
		{
			if (percent <= 10)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 10 && percent <= 60)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 60 && percent <= 65)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 65 && percent <= 70)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 70)
			{
				curr_weather = SKY_SNOW;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_LIGHTNING)
		{
			if (percent <= 5)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 5 && percent <= 85)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 85 && percent <= 95)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 95)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_HAIL)
		{
			if (percent <= 5)
			{
				curr_weather = SKY_LIGHTNING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent < 5 && percent <= 55)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 55 && percent <= 65)
			{
				curr_weather = SKY_RAINING;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 65 && percent <= 70)
			{
				curr_weather = SKY_HAIL;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 70)
			{
				curr_weather = SKY_SNOW;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
		if (old_weather == SKY_SNOW)
		{
			if (percent <= 25)
			{
				curr_weather = SKY_CLOUDY;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
			if (percent > 25)
			{
				curr_weather = SKY_SNOW;
				fprintf (fp, "%d\r", curr_weather);
				break;
			}
		}
	}
	old_weather = curr_weather;

	}

	fclose (fp);

}



////////////////////////////////////////////////////////////////////////////////////
// Function: Load_weather
// Usage   : Called at bootup and every PULSE_WEATHER to read a new weather formation
//		   : in from the weather.dat file
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

bool load_weather(void)
{
	FILE *fp;
	int number = 0;
	int weather = 0;
	int i = -1;
	char file[MIL];
	
	sprintf (file, "%s%s", DATA_DIR, WEATHER_FILE);
	
	if (!(fp = fopen (file, "r")))
		return FALSE;

	// Back up old weather so we can have messages for repeated
	// conditions
	weather_info.old_sky = weather_info.sky;
		
	for (;;)
	{
		if (feof (fp))
		{
			fclose (fp);
			return FALSE;
		}
		weather = fread_number(fp);
		i++;

		if (i == weather_info.change)
		{
			fclose (fp);

			switch (weather)
			{
			case SKY_CLOUDY: weather_info.sky = SKY_CLOUDY;
				break;
			case SKY_CLOUDLESS: weather_info.sky = SKY_CLOUDLESS;
				break;
			case SKY_HAIL: weather_info.sky = SKY_HAIL;
				break;
			case SKY_RAINING: weather_info.sky = SKY_RAINING;
				break;
			case SKY_SNOW: weather_info.sky = SKY_SNOW;
				break;
			case SKY_LIGHTNING: weather_info.sky = SKY_LIGHTNING;
				break;
			}
			return TRUE;
		}
		
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////
// Function: Weatherstate_update
// Usage   : Function which updates the weather every PULSE_WEATHER which is an hour
//			 real life, and a day mud time.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void weatherstate_update(void)
{
	DESCRIPTOR_DATA *d;
	char buf[MSL];
	char inside[MSL];
	char city[MSL];
	char forest[MSL];
	char hills[MSL];
	char mountain[MSL];
	char water[MSL];
	char desert[MSL];
	char grassland[MSL];
	char jungle[MSL];
	char vantage[MSL];
	char river[MSL];
	char swamp[MSL];
	char road[MSL];
	char ocean[MSL];
	char beach[MSL];

	memset (inside, 0, MSL);
	memset (city, 0, MSL);
	memset (forest, 0, MSL);
	memset (hills, 0, MSL);
	memset (mountain, 0, MSL);
	memset (water, 0, MSL);
	memset (desert, 0, MSL);
	memset (grassland, 0, MSL);
	memset (jungle, 0, MSL);
	memset (vantage, 0, MSL);
	memset (river, 0, MSL);
	memset (swamp, 0, MSL);
	memset (road, 0, MSL);
	memset (ocean, 0, MSL);
	memset (beach, 0, MSL);
	memset (buf, 0, MSL);


	d = descriptor_list;

	if (d == NULL)
		return;

	// First we need to check if change is equal to or greater
	// than 192, if it is, we need to create a new weather table.
	if (weather_info.change >= 192)
	{
		// Write a new weather table and set create to 1
		weather_create();
		weather_info.change = 1;
	}
	
	// Update: Now we check to see if there are any weather changes made by players
	// that need to be made
	if (weather_info.duration > 0)
	{
		// Set the weather type
		weather_info.sky = weather_info.wchange;
		weather_info.old_sky = weather_info.sky;
		// Reduce the duration by one
		weather_info.duration--;

		// Check if this makes the duration 0, hence nulifying the weather change
		// next pulse_weather
		if (weather_info.duration == 0)
		{
			weather_info.wchange = 0;
			weather_info.power = 0;
		}
	}

	if (weather_info.duration <= 0)
	{
		// Now we load in the current weather pattern
		load_weather();
	}

	// Increase the weather by one ready for the next PULSE_WEATHER
	weather_info.change++;

	// Now we need to display a weather message to all players
	// For the time being we will consider that all players are on the
	// same island

	// The Age of Stone Weather Message System (tm) 
	// Written by: Owen Varley

	// Messages for a repeat of the same weather condition
	if (weather_info.sky == weather_info.old_sky)
	{
	switch(weather_info.sky)
	{
	case SKY_CLOUDLESS:
		{
			// Now Sector depedant messages
			strcat (city, "The sky remains clear in all directions as the warm sun illuminates the city.\n\r");
			strcat (forest, "The sun's rays illuminate the forest floor as they break through the light canopy, the sky above remains crystal clear.\n\r");
			strcat (hills, "The sun continues to grace the hills with warmth as there is still not a cloud in sight.\n\r");
			strcat (mountain, "High up in the mountains the Sun continues to grace the land with its presence.\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "The blazing sun continues to bare down on the sands, theres not a cloud visible in the sky.\n\r");
			strcat (grassland, "The sun continues to shine down on the grasslands with no signs of clouds.\n\r");
			strcat (jungle, "The sun's rays continue to break through the thick jungle canopy with minor sucess.\n\r");
			strcat (vantage, "From here the sun is clearly visible as it continues to shine in the clear blue sky.\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "The otherwise dark and dull swamps are still illuminated by the blazing sun.\n\r");
			strcat (road, "The sun remains high up in the sky, gracing the road with its presence.\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The sunshine continues to bathe the beach in glorious yellow light.\n\r");
	
		break;
		}
	case SKY_CLOUDY:
		{
			// Now Sector depedant messages
			strcat (city, "The dismal weather continues with the clouds above the city obscuring the sun.\n\r");
			strcat (forest, "The clouds continue to deny the forest of the sun's rays.\n\r");
			strcat (hills, "The clouds continue to barr the sunlight from the hills.\n\r");
			strcat (mountain, "The mountains are cast in shadow as the sun still hides behind the clouds.\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "The sun remains hidden in the clouds, sparing those walking the sands.\n\r");
			strcat (grassland, "The grasslands remain without sunlight as the sun hides behind the clouds.\n\r");
			strcat (jungle, "The jungle remains dark as the sun which barely penetrates the canopy is blocked by the cloud cover.\n\r");
			strcat (vantage, "The cloudy sky continues to prevent the sun from illuminating the area.\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "The cloud-hidden sun continues to leave the swamps their usual dismal dark.\n\r");
			strcat (road, "The road remains with out sunlight as the sun continues to be hidden by the clouds.\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The dismal weather continues to keep the sun from touching the beach sands.\n\r");
	
		break;
		}
	case SKY_RAINING:
		{
			// Now Sector depedant messages
			strcat (city, "The city continues to be soaked through by the rains.\n\r");
			strcat (forest, "The sounds of the forest trees intercepting the rain continues to resonate.\n\r");
			strcat (hills, "The rain continues its relentless downpour, drenching the hilly area.\n\r");
			strcat (mountain, "The mountains remain wet and slippery as the rain continues without mercy.\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "Luck graces the sands as the rain continues to pour down from the heavens!\n\r");
			strcat (grassland, "The grasslands remain slick with rain as it continues to downpour.\n\r");
			strcat (jungle, "The soft sound of the rain trying to break through the Jungle's canopy remains far overhead.\n\r");
			strcat (vantage, "The miserable and wet weather continues with relentless rain pouring down.\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "The swamp remains characteristically wet as the rain continues to fall down.\n\r");
			strcat (road, "The road remains wet and sodden as the rain continues to fall.\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The sands remain wet and hard as the rain continues to fall down.\n\r");
	
		break;
		}
	case SKY_SNOW:
		{
			// Now Sector depedant messages
			strcat (city, "The soft white snow continues to blanket the buildings and houses of the city.\n\r");
			strcat (forest, "Snow continues to fall, gradually blanketing the forest floor with white.\n\r");
			strcat (hills, "The snow continues to fall, adding to the already white hill tops.\n\r");
			strcat (mountain, "The snow continues to build up on the mountains as it falls from the heavens.\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "Miracles do happen! The snow continues to cover the desert sands!\n\r");
			strcat (grassland, "Snow from the skies continues to cover the grasslands in white.\n\r");
			strcat (jungle, "The snow continues to fall, what little snow makes it through the canopy ends up as piles of water on the jungle floor.\n\r");
			strcat (vantage, "Snow continues to fall in all directions making vision even more difficult.\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "The swamps continue to be assaulted by falling snow flakes as they slowly cover the swamp in white.\n\r");
			strcat (road, "The road conditions worsen as the snow continues to cover its surface in white.\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The beach sands remain blanketed in thick white snow as it continues to fall from the sky.\n\r");
	
		break;
		}
	case SKY_LIGHTNING:
		{
			// Now Sector depedant messages
			strcat (city, "Lightning continues to flash from the sky and thunder rolls as the storm continues to assault the city.\n\r");
			strcat (forest, "The storm continues to rage on over head as lightning cracks and thunder rolls!\n\r");
			strcat (hills, "Pelting rain, lightning bolts and thunder claps continue to ravage the hills!\n\r");
			strcat (mountain, "The Lightning continues to crash down dangerously close as the mountains continue to be attacked by the heavens!\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "Hollow booms of thunder and flashes of lightning continue to plague the desert sands!\n\r");
			strcat (grassland, "The storm remains above the grasslands, continue its assault of light and sound!\n\r");
			strcat (jungle, "Even through the thick canopy of the jungle, the continuing storm is experienced!\n\r");
			strcat (vantage, "Lightning bolts continue to fall dangerously close as the storm rages uncontrolably!\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "The continuing storm crashes around the swamp lashing out with lightning and thunder.\n\r");
			strcat (road, "The roads continue to experience the wrath of the heavens as lightning falls and thunder crashes!\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The beach sands remain in the storm as it continues to pelt down with rain, lightning and thunder!\n\r");
	
		break;
		}
	case SKY_HAIL:
		{
			// Now Sector depedant messages
			strcat (city, "The skies continue to pelt down hailstones on the city!\n\r");
			strcat (forest, "Hailstones continue to break through the forest's canopy!\n\r");
			strcat (hills, "Up high on the hills, the hail continues to lash down!\n\r");
			strcat (mountain, "The towering mountains continue to be assaulted by the hailstorm!\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "Surprisingly, the sands of the desert continue to be attacked by hailstones!\n\r");
			strcat (grassland, "The hailstones continue to fall in the grasslands without remorse!\n\r");
			strcat (jungle, "The jungle's canopy continues to act as a shield from the falling hailstones!\n\r");
			strcat (vantage, "The stinging hailstones continue to rocket down on the area!\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "The swamps remain under assualt from the hailstorm!\n\r");
			strcat (road, "Hailstones continue to fall upon the road!\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "Solid hailstones continue to fall on the beach sands!\n\r");
	
		break;
		}
	}
	}
	else
	if (weather_info.sky != weather_info.old_sky)
	{
	switch(weather_info.sky)
	{
	// Weather messages now take the previous weather patterns into consideration
	// as well as the terrain
	case SKY_CLOUDLESS:
		{
			strcat (city, "High above the city, the Sun breaks through the clouds raining down glorious sunshine.\n\r");
			strcat (forest, "Light returns to the forest's floor as the sun breaks through the clouds.\n\r");
			strcat (hills, "The hills are once more bathed in sunlight as the sun emerges from the clouds.\n\r");
			strcat (mountain, "Sunlight once more graces the mountains as the sun emerges from the clouds.\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "The sun emerges from the clouds, returning to bake the desert sands.\n\r");
			strcat (grassland, "The sun returns to the grasslands, breaking through the cloud cover.\n\r");
			strcat (jungle, "Light returns to the Jungle as the sun emerges into clear skies.\n\r");
			strcat (vantage, "The sun returns having broken through the clouds.\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "The swamps are once more lit by the sun as it clears the clouds.\n\r");
			strcat (road, "The sun returns to the road after having broken free from the clouds.\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The sun's glorious rays once more bathe the beach in light as it clears the clouds.\n\r");
		break;
		}
	case SKY_CLOUDY:
		{
			if (weather_info.old_sky == SKY_CLOUDLESS)
			{
			strcat (city, "The clouds arrive turning the almost perfect weather above the city sour.\n\r");
			strcat (forest, "The forest floor darkens as the clouds consume the sun.\n\r");
			strcat (hills, "The sun disappears before the clouds as the hill side is dropped into darkness.\n\r");
			strcat (mountain, "The mountain becomes slightly darker as the sun is hidden by the clouds.\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "The baking desert sun disappears into the clouds.\n\r");
			strcat (grassland, "The grasslands sway gentle as the sun vanishes into the clouds.\n\r");
			strcat (jungle, "The Jungle is pitched into darkness as the clouds consume the sun.\n\r");
			strcat (vantage, "The clouds swallow the sun casting down a dark shadow.\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "With the sun disappearing into the clouds, the swamps once again become murky\n\r");
			strcat (road, "The roads darken as the sun is taken over by the clouds.\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The glorious beach weather turns sour as the sun is consumed by clouds.\n\r");
			}

			if (weather_info.old_sky == SKY_RAINING)
			{
			strcat (city, "The rains show mercy to the city as they let up, leaving the sky cloudy and dark.\n\r");
			strcat (forest, "The forest floor darkens as the clouds consume the sun.\n\r");
			strcat (hills, "The sun disappears before the clouds as the hill side is dropped into darkness.\n\r");
			strcat (mountain, "The mountain becomes slightly darker as the sun is hidden by the clouds.\n\r");
			strcat (water, ".\n\r");
			strcat (desert, "The baking desert sun disappears into the clouds.\n\r");
			strcat (grassland, "The grasslands sway gentle as the sun vanishes into the clouds.\n\r");
			strcat (jungle, "The Jungle is pitched into darkness as the clouds consume the sun.\n\r");
			strcat (vantage, "The clouds swallow the sun casting down a dark shadow.\n\r");
			strcat (river, ".\n\r");
			strcat (swamp, "With the sun disappearing into the clouds, the swamps once again become murky\n\r");
			strcat (road, "The roads darken as the sun is taken over by the clouds.\n\r");
			strcat (ocean, ".\n\r");
			strcat (beach, "The glorious beach weather turns sour as the sun is consumed by clouds.\n\r");
			}

		strcat (buf, "The skies turn dark and cloudy, the sun barely managing to break through the thick clouds.\n\r");
		break;
		}
	case SKY_RAINING: strcat (buf, "Rain starts to trickle from the skies, showering the lands in water.\n\r");
		break;
	case SKY_SNOW: strcat (buf, "Snow starts falling, blanketing the lands in white.\n\r");
		break;
	case SKY_LIGHTNING: strcat (buf, "With powerful booms of thunder and crashes of lightning the rain turns its nasty side as a storm brews!\n\r");
		break;
	case SKY_HAIL: strcat (buf, "Thick hailstones fall from the sky as a hailstorm starts up.\n\r");
		break;
	}
	}


//	if (buf[0] != '\0')
//	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING
				&& !IS_NPC (d->character)
				&& IS_OUTSIDE (d->character)
				&& IS_AWAKE (d->character)
				&& !IS_FLAG (d->character->states.config, _NOWEATHER))
			{
				switch(d->character->in_room->sector_type)
				{
				case SECT_CITY: send_to_char (city, d->character);
					break;
				case SECT_FOREST: send_to_char (forest, d->character);
					break;
				case SECT_HILLS: send_to_char (hills, d->character);
					break;
				case SECT_MOUNTAIN: send_to_char (mountain, d->character);
					break;
				case SECT_WATER: send_to_char (water, d->character);
					break;
				case SECT_DESERT: send_to_char (desert, d->character);
					break;
				case SECT_GRASSLAND: send_to_char (grassland, d->character);
					break;
				case SECT_JUNGLE: send_to_char (jungle, d->character);
					break;
				case SECT_VANTAGE: send_to_char (vantage, d->character);
					break;
				case SECT_RIVER: send_to_char (river, d->character);
					break;
				case SECT_SWAMP: send_to_char (swamp, d->character);
					break;
				case SECT_ROAD: send_to_char (road, d->character);
					break;
				case SECT_OCEAN: send_to_char (ocean, d->character);
					break;
				case SECT_BEACH: send_to_char (beach, d->character);
					break;
				default: send_to_char (city, d->character);
					break;
				}
				continue;

			}
		}
//	}
	
}

////////////////////////////////////////////////////////////////////////////////////
// Function: Load_time
// Usage   : Called at startup to load in the saved time details from the time.dat
//		   : file.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void load_time (void)
{
	FILE *fp;
	int number = 0;
	char file[MIL];
	
	sprintf (file, "%s%s", DATA_DIR, TIME_FILE);
	
	if (!(fp = fopen (file, "r")))
		return;
	
	for (;;)
	{
		char *word;
		char letter;
		
		do
		{
			letter = getc (fp);
			if (feof (fp))
			{
				fclose (fp);
				return;
			}
		}
		while (isspace (letter));
		ungetc (letter, fp);
		
		word = fread_word (fp);
		
		if (!str_cmp (word, "SEASON"))
		{
			number = fread_number (fp);
			if (number > 0)
				time_info.season = number;

			number = 0;
		}
		if (!str_cmp (word, "DAY"))
		{
			number = fread_number (fp);
			if (number > 0)
				time_info.day = number;

			number = 0;
		}
		if (!str_cmp (word, "DAYC"))
		{
			number = fread_number (fp);
			if (number > 0)
				time_info.day_c = number;

			number = 0;
		}
		if (!str_cmp (word, "MONTH"))
		{
			number = fread_number (fp);
			if (number > 0)
				time_info.month = number;

			number = 0;
		}
		if (!str_cmp (word, "YEAR"))
		{
			number = fread_number (fp);
			if (number > 0)
				time_info.year = number;

			number = 0;
		}
		if (!str_cmp (word, "WEATHER"))
		{
			number = fread_number (fp);
			if (number > 0)
				weather_info.change = number;

			number = 0;
		}
		if (!str_cmp (word, "POWER"))
		{
			number = fread_number (fp);
			if (number > 0)
				weather_info.power = number;

			number = 0;
		}
		if (!str_cmp (word, "CHANGE"))
		{
			number = fread_number (fp);
			if (number > 0)
				weather_info.wchange = number;

			number = 0;
		}
		if (!str_cmp (word, "DURATION"))
		{
			number = fread_number (fp);
			if (number > 0)
				weather_info.duration = number;

			number = 0;
		}
		if (!str_cmp (word, "PROTECTION"))
		{
			number = fread_number (fp);
			if (number > 0)
				weather_info.prot = number;

			number = 0;
		}
	}
	fclose (fp);
}

////////////////////////////////////////////////////////////////////////////////////
// Function: Time_update
// Usage   : Called every tick to update the time, month, season, year, etc
//			 after it updates the time, it saves it all into the time.dat file
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void time_update(void)
{
	char file[MIL];
	FILE *fp;
	char buf[MSL];

	DESCRIPTOR_DATA *d;
	int trigger = -1;

#ifdef _DEBUG
	long start_t = 0, end_t = 0;
	static long last_update;

	start_t = time(NULL);
#endif

	memset(buf, 0, MSL);

	// Changing the time and showing messages
	switch (++time_info.hour)
	{
		case 5:
			trigger = SOUND_AREA_SUN_LIGHT;
			weather_info.sunlight = SUN_LIGHT;
			strcat (buf, "The day has begun.\n\r");
			break;
			
		case 6:
			trigger = SOUND_AREA_SUN_RISE;
			weather_info.sunlight = SUN_RISE;
			strcat (buf, "The sun rises in the east.\n\r");
			break;
			
		case 19:
			trigger = SOUND_AREA_SUN_SET;
			weather_info.sunlight = SUN_SET;
			strcat (buf, "The sun slowly disappears in the west.\n\r");
			break;
			
		case 20:
			trigger = SOUND_AREA_SUN_DARK;
			weather_info.sunlight = SUN_DARK;
			strcat (buf, "The night has begun.\n\r");
			break;
			
		case 24:
			time_info.hour = 0;
			time_info.day++;
			time_info.day_c++;
			break;
	}

	// Reset time counter and increase month by one
	if (time_info.day >= 35)
	{
		time_info.day = 0;
		time_info.month++;
	}

	// Time to change the season
	// If mud crashes we loose time and season information
	// Need to have this saved to time.dat file at a later date.
	if (time_info.day_c >= 45)
	{
		time_info.day_c = 0;
		switch(time_info.season)
		{
		case SEASON_SUMMER: time_info.season = SEASON_AUTUMN;
							break;
		case SEASON_AUTUMN: time_info.season = SEASON_WINTER;
							break;
		case SEASON_WINTER: time_info.season = SEASON_SPRING;
							break;
		case SEASON_SPRING: time_info.season = SEASON_SUMMER;
							break;
		}		
	}
	
	// Reset month counter and increase year counter
	if (time_info.month >= 17)
	{
		time_info.month = 0;
		time_info.year++;
	}

	// Show the time message to the player
	// Will be moved to end of function later to show weather as well
	if (buf[0] != '\0')
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING
				&& !IS_NPC (d->character)
				&& IS_OUTSIDE (d->character)
				&& IS_AWAKE (d->character)
				&& !IS_FLAG (d->character->states.config, _NOWEATHER))
			{
				if (trigger != -1 && d->character->zone)
					snd_trigger (d->character, d->character->zone->sounds, SOUND_AREA,trigger, TRUE);

				send_to_char (buf, d->character);
				
				if ((weather_info.sunlight == SUN_LIGHT ||
					weather_info.sunlight == SUN_DARK ||
					weather_info.sunlight == SUN_SET)
					&& IS_AFFECTED (d->character, _AFF_DARK_VISION))
				{
					send_to_char ("Your eyes take a moment to adjust to the"
						" changing light.\n\r", d->character);
				}
			}
		}
	}

	// Saving changed time and date in dat file
	sprintf (file, "%s%s", DATA_DIR, TIME_FILE);
	if (!(fp = fopen (file, "w")))
	{
		bugf ("[weather.c::weatherstate_update] fopen of %s failed", file);
		return;
	}
	fprintf (fp, "SEASON %d\r", time_info.season);
	fprintf (fp, "DAY %d\r", time_info.day);
	fprintf (fp, "DAYC %d\r", time_info.day_c);
	fprintf (fp, "MONTH %d\r", time_info.month);
	fprintf (fp, "YEAR %d\r", time_info.year);
	fprintf (fp, "WEATHER %d\r", weather_info.change);
	fprintf (fp, "POWER %d\r", weather_info.power);
	fprintf (fp, "CHANGE %d\r", weather_info.wchange);
	fprintf (fp, "DURATION %d\r", weather_info.duration);
	fprintf (fp, "PROTECTION %d\r", weather_info.prot);
	fclose (fp);
}

////////////////////////////////////////////////////////////////////////////////////
// Function: Move_weather
// Usage   : Used to move tornadoes around the world, they move in a random acceptable
//		   : direction every tick.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void move_weather (OBJ_DATA *obj)
{

	//obj_to_room (weather, victim->in_room);
}

////////////////////////////////////////////////////////////////////////////////////
// Function: Tornado
// Usage   : Tornadoes are created when two consequitive storms take place, they move
//		   : around the world throwing people around and causing damage. This function
//		   : handles their creation.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void tornado(void)
{
	OBJ_DATA *tornado;
	DESCRIPTOR_DATA *d;
	int location;
				
	// For a tornado to form we need to have two storms in a row
	if (weather_info.old_sky == SKY_LIGHTNING)
	{
		// Determine spawn location of tornado
		location = get_room_index(number_range(3000, ROOM_ISLAND));	
		
		if (location == NULL)
			return;
	
		// We need to create the tornado object
		tornado = create_object (get_obj_index (OBJ_VNUM_TORNADO), 0);
		tornado->timer = random_dice(2, 25, 0);
		obj_to_room(tornado, location);

	for (d = descriptor_list; d; d = d->next)
	{
		if (d->connected == CON_PLAYING)
		{
			// Only send message to those in the same room as Tornado
			if (d->character->in_room == tornado->in_room)
			{
				send_to_char ("With high velocity winds sweeping the area, a Tornado spins into existance! Destroying all in its path! RUN!", d->character);
				send_to_char ("\n\r", d->character);
			}
			// Display message for those people in the surrounding rooms
			if (leads_to(d->character->in_room, tornado->in_room))
			{
				send_to_char("Swirling winds combine forming a vicious tornado closeby! RUN!\n\r", d->character);
			}
		}
	}		

		return;
		}
//	}
	return;

}

////////////////////////////////////////////////////////////////////////////////////
// Function: Aff_tornado
// Usage   : Function which handles the affects of being in the same room as a tornado
//		   : or being swept up in one.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void aff_tornado(CHAR_DATA *ch)
{
	sh_int attempt;
	sh_int chuck;
	sh_int num = number_range(1, 3);
	switch (num)
	{
	case 1: send_to_char("You walk straight into the path of a Tornado! It throws you around the room relentlessly!\n\r", ch);
			act ("The raging tornado whisks $n up in its swirling winds before tossing $m out of the room!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			break;
	case 2: send_to_char("You foolishly step directly into the path of a Tornado! It whisks you up into its swirling winds!\n\r", ch);
			act ("The raging tornado whisks $n up in its swirling winds before tossing $m out of the room!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			break;
	case 3: send_to_char("Without realising it you step directly into the path of the Tornado! It picks you up and tosses you about without mercy!\n\r", ch);
			act ("The raging tornado whisks $n up in its swirling winds before tossing $m out of the room!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			break;
	}

	// If passes a 1d10 > 5 check
	// Chuck them in a random direction
	// If they dont get chucked there is a random chance that they may get hit
	// If there are objects in the room.
	chuck = random_dice(1, 10, 0);
	if (chuck >= 6);
	{
	for (attempt = 0; attempt < 6; attempt++)
	{
		EXIT_DATA *pexit;
		int i;
		
		i = number_door ();
		if ((pexit = ch->in_room->exit[i]) == 0
			|| pexit->u1.to_room == NULL
			|| IS_SET (pexit->exit_info, EX_CLOSED)
			|| (IS_NPC (ch)
			&& IS_SET (pexit->u1.to_room->room_flags, ROOM_NO_MOB)))
			continue;
		
		move_char (ch, i, FALSE);
		break;
	}
		// Dump them on the ground
		ch->position = POS_RESTING;
		// Give them a message
		send_to_char("THUMP!! You land hard on the ground after being thrown clear of the tornado!\n\r", ch);
		act("With a loud THUMP $n lands in a heap, having been thrown clear of the looming tornado!", ch, NULL, NULL, TO_ROOM,	POS_RESTING);
		// Randomly damage them
		ch->hit -= number_range(1, 25);
		return;
	
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////////
// Function: Lightning
// Usage   : Function which handles deadly lightning strikes which occur every 1d20 seconds
//		   : in a storm.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void lightning(void)
{
	if (weather_info.sky == SKY_LIGHTNING)
	{

	ROOM_INDEX_DATA *location;
	DESCRIPTOR_DATA *d, *r;
	int room_roll = 0;
	int death_roll = 0;
	int damage = 0;
	int roll = 0;
	int check_message = 0;

	// Temporary range will change in all weather when we complete island
	location = get_room_index(number_range(3014, ROOM_ISLAND));

	room_roll = number_range(1, 5);
	
	for (r = descriptor_list; r; r = r->next)
	{

	if (r->connected == CON_PLAYING)
		{

		// This loop is used to display messages to players in vicinity
		if (leads_to (r->character->in_room, location))
		{
			send_to_char("A bolt of lightning leaps from the skies! Crashing down close by!\n\r", r->character);
		}		
		if (check_message =! 1);
		{
		if (r->character->in_room->vnum == location->vnum)
		{
				// Display a message of that lightning bolt coming into existance
				switch(room_roll)
				{
				case 1: act("A blazing bolt of pure yellow light slashes out of the storm clouds!", r->character, NULL, NULL, TO_ROOM, POS_RESTING);
					act("A blazing bolt of pure yellow light slashes out of the storm clouds!", r->character, NULL, NULL, TO_CHAR, POS_RESTING);
					break;
				case 2: act("A bright white fork of lightning lashes out from the storm clouds overhead!", r->character, NULL, NULL, TO_ROOM, POS_RESTING);
					act("A blazing bolt of pure yellow light slashes out of the storm clouds!", r->character, NULL, NULL, TO_CHAR, POS_RESTING);
					break;
				case 3: act("A lightning bolt bursts from the heavens in a blinding flash of light!", r->character, NULL, NULL, TO_ROOM, POS_RESTING);
					act("A blazing bolt of pure yellow light slashes out of the storm clouds!", r->character, NULL, NULL, TO_CHAR, POS_RESTING);
					break;
				case 4: act("A jagged lightning bolt cracks out from the thick storm clouds above!", r->character, NULL, NULL, TO_ROOM, POS_RESTING);
					act("A blazing bolt of pure yellow light slashes out of the storm clouds!", r->character, NULL, NULL, TO_CHAR, POS_RESTING);
					break;
				case 5: act("The heavens strike out! A bolt of lightning crackles down towards the ground!", r->character, NULL, NULL, TO_ROOM, POS_RESTING);
					act("A blazing bolt of pure yellow light slashes out of the storm clouds!", r->character, NULL, NULL, TO_CHAR, POS_RESTING);
					break;
				}
			check_message = 1;
			}
		}
		}
	}

	for (d = descriptor_list; d; d = d->next)
	{
		if (d->connected == CON_PLAYING)
		{
			// Give players at the lightning strike location a message
			// those in surrounding rooms get given a message as well
			if (d->character->in_room->vnum == location->vnum)
			{
				// They get hit!
				// Damage from 50 to 200
				damage = random_dice(50, 4, 0);

				// Give them a nice message
				roll = number_range(1, 5);
					
				switch (roll)
				{
				case 1: send_to_char("Electricity arcs through your body followed by waves of pain as the Lightning bolt consumes you!\n\r", d->character);
					act("$n is consumed by pure white light as $s is struck by a bolt of lightning!", d->character, NULL, NULL, TO_ROOM, POS_RESTING);
					break;
				case 2: send_to_char("You writhe in pain as your body is super heated as your are struck by Lightning!\n\r", d->character);
					act("$n is consumed by pure white light as $s is struck by a bolt of lightning!", d->character, NULL, NULL, TO_ROOM, POS_RESTING);
					break;
				case 3: send_to_char("Heat waves wrack your body as the lightning bolt strikes you! Blinding you with its bright light!\n\r", d->character);
					act("The blaze of light consumes $n, $s screams with pain and writhes violently!", d->character, NULL, NULL, TO_ROOM, POS_RESTING);
					break;
				case 4: send_to_char("Your body dances as if possesed as the lightning bolt strikes you! Pain flows through you!\n\r", d->character);
					act("The lightning bolt strikes $n as $s writhes in obvious pain, wracked by heat from the bolt!", d->character, NULL, NULL, TO_ROOM, POS_RESTING);
					break;
				case 5: send_to_char("Consumed by Light you scream in pain! The Lightning bolt lashes down through your body!\n\r", d->character);
					act("$n is struck by the bolt of Lightning! Screaming $s is consumed in pure white light!", d->character, NULL, NULL, TO_ROOM, POS_RESTING);
					break;
				}
						
				d->character->hit -= damage;
				
				// Need to update state and stuff
				update_pos(d->character);

				switch (d->character->position)
				{
					
				case POS_MORTAL:
					if (d->character->wtimer == 0)
					{
						act ("Badly burned and in shock, $n collapses as the bolt of lightning evaporates! .",
							d->character, NULL, NULL, TO_ROOM, POS_RESTING);
						send_to_char ("The lightning bolt vanishes as you collapse! Badly Burnt!\n\r",
							d->character);
						
						d->character->wtimer = 1;
						stop_fighting (d->character, TRUE);
					}
					break;
					
				case POS_INCAP:
					act ("$n crumples to the ground burned badly by the bolt of lightning!",
						d->character, NULL, NULL, TO_ROOM, POS_RESTING);
					send_to_char
						("You crumble to the floor as the lightning bolt dispaits leaving you badly wounded!\n\r",
						d->character);
					break;
					
				case POS_STUNNED:
					act ("$n stumbles about stunned after being struck by lightning!",
						d->character, NULL, NULL, TO_ROOM, POS_RESTING);
					send_to_char ("The Creator must look down open you with mercy! You are mearly stunned!\n\r", d->character);
					break;
					
				case POS_DEAD:
					death_roll = random_dice(1, 5, 0);
					switch (death_roll)
					{
					case 1:
						act ("$n screams as the Lightning bolt burns $m to a crisp!!", d->character, 0, 0, TO_ROOM, POS_RESTING);
						break;
					case 2:
						act ("$n's body explodes, blown to pieces by the Lightning Strike!", d->character, 0, 0, TO_ROOM, POS_RESTING);
						break;
					case 3:
						act ("$n screams inhumanily in pain before $s charred corpse collapses to the ground!", d->character, 0, 0, TO_ROOM, POS_RESTING);
						break;
					case 4:
						act ("$n writhes in pain, as $e crumbles to the ground, dead!", d->character, 0, 0, TO_ROOM, POS_RESTING);
						break;
					case 5:
						act ("$n's body is ripped to pieces by the Lightning Strike!", d->character, 0, 0, TO_ROOM, POS_RESTING);
						break;
					}
					send_to_char ("You have been KILLED!!\n\r\n\r", d->character);
					break;

		}

		}
		}
		

	} 
}
}

void tidal(void)
{

}

// Weather prediction with the
// correct ability

////////////////////////////////////////////////////////////////////////////////////
// Function: do_predictweather
// Usage   : Predict weather function, allows players with the correct ability
//		   : to detect changes in the weather and to predict what the weather will
//		   : be like.
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

_DOFUN(do_predictweather)
{
	char arg[MIL];
	int time;
	int selection;
	int weather;
	int i = 0;
	FILE *fp;
	char file[MIL];
	
	one_argument (argument, arg);

	time = atoi (arg);

	if (ch->ability[ABILITY_PREDWEATHER] != 1)
	{
		send_to_char("Your senses are not fine tuned to the weather.\n\r", ch);
		return;
	}

	if (time == 0)
	{
		send_to_char("You need to enter a time to predict\n\r", ch);
		return;
	}
	if (time > 6)
	{
		send_to_char("You can only predict up to 6 days in advance (1 - 6)\n\r", ch);
		return;
	}

	// Add the time to the current weather change variable to get the prediction
	// we need
	selection = weather_info.change + time;

	// Need to check to see if we have predicted over the maximum created weather slots
	if (selection > 192)
	{
		send_to_char("The `CWinds`w are Shifting once again, and you are unable to Forecast until they take course.\n\r", ch);
		return;
	}

	// Open weather file to read in prediction
	sprintf (file, "%s%s", DATA_DIR, WEATHER_FILE);
	
	if (!(fp = fopen (file, "r")))
		return;

	for (;;)
	{
		if (feof (fp))
		{
			fclose (fp);
			return;
		}
		weather = fread_number(fp);
		i++;

		if (i == selection)
		{
			// Close file
			fclose (fp);
			// Give message
			switch (weather)
			{
			case SKY_CLOUDY: send_to_char("Your senses forecast the weather as cloudy but calm\n\r", ch);
				break;
			case SKY_CLOUDLESS: send_to_char("Your senses forecast a perfect calm and clear day!\n\r", ch);
				break;
			case SKY_HAIL: send_to_char("Your senses forecast rapid hailstorms!\n\r", ch);
				break;
			case SKY_RAINING: send_to_char("Your senses forecast a miserable rainy downpour!\n\r", ch);
				break;
			case SKY_SNOW: send_to_char("Your senses forecast white snow blanketing the island!\n\r", ch);
				break;
			case SKY_LIGHTNING: send_to_char("Your senses forecast a dark and stormy day!\n\r", ch);;
				break;
			}

			return;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Function: weather_change
// Usage   : Function used when a muscian tries to change the weather with a song
// Author  : Owen Varley (owen@interactiontraining.com)							
////////////////////////////////////////////////////////////////////////////////////

void weather_change(int power, int change)
{
	DESCRIPTOR_DATA *d;
	int message;

	// First we check whether the power of the player wanting to change the weather's
	// song is stronger than any current changes imposed
	if (weather_info.power >= power)
	{
		return;
	}

	// Next we check that the power of the players song is stronger than any protections
	// that may be in place
	if (weather_info.prot >= power)
	{
		return;
	}

	// Ok so the player can now change the weather, so we set the new weather for 
	// the next weather update, setting the duration, power and type
	weather_info.power = power;
	weather_info.prot = 0;
	weather_info.duration = random_dice(1, 10, 0);
	weather_info.wchange = change;

	message = random_dice(1, 5, 0);
	// Any players with the predictweather ability get a message
	for (d = descriptor_list; d; d = d->next)
	{
		if (d->connected == CON_PLAYING)
		{
			if (d->character->ability[ABILITY_PREDWEATHER] == 1)
			{
				switch( message )
				{
				case 1: send_to_char("You sneeze suddenly and violently!\n\r", d->character);
					break;
				case 2: send_to_char("Your joints ache all of sudden, for no apparent reason!\n\r", d->character);
					break;
				case 3: send_to_char("You sniff as your nose starts to run without reason!\n\r", d->character);
					break;
				case 4: send_to_char("Your head starts to ache as a headache forms without case!\n\r", d->character);
					break;
				case 5: send_to_char("You cough repeatedly as your throat feels dry!\n\r", d->character);
					break;
				}
			}
		}
	}

	// Weather has been changed so return
	return;


}