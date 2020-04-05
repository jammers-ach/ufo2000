AddXcomTerrain {
	Name = "Cydonia Base",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/u_base.*",
		"$(xcom)/terrain/u_wall02.*",
		"$(xcom)/terrain/u_pods.*",
		"$(xcom)/terrain/brain.*",
		"$(xcom)/terrain/mars.*"
	},
	Maps = {
		"$(extension)/cydoniabase00.map",
		"$(extension)/cydoniabase01.map",
		"$(extension)/cydoniabase02.map",
		"$(extension)/cydoniabase03.map"

	},
	MapGenerator = function(map)

		if (map.SizeX ~= 6) then map.SizeX = 6 end
		
		if (map.SizeY ~= 6) then map.SizeY = 6 end

		if ((map.SizeX == 6) and (map.SizeY == 6)) then
   			  local map_number = math.random(1, 4)

           if map_number == 1 then
               	map. Mapdata = {
		    { 00, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }

           elseif map_number == 2 then
               map.Mapdata = {
		    { 01, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       
 
	   elseif map_number == 3 then
               map.Mapdata = {
		    { 02, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       

	   elseif map_number == 4 then
               map.Mapdata = {
		    { 03, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
                { -1, -1, -1, -1, -1, -1 },
               }       

           end

           return map

    
   end
   end


}