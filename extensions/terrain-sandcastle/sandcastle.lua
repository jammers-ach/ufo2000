AddXcomTerrain {
	Name = "X-COM Sandcastle",
	Tiles =	{
		"$(xcom)/terrain/BLANKS.*",
		"$(xcom)/terrain/DESERT.*",
		"$(xcom)/terrain/XBASE1.*",
		"$(xcom)/terrain/XBASE2.*"
	},
	Maps = {
		"$(extension)/sandcastle00.map"
	},

	MapGenerator = function(map)

		if (map.SizeX ~= 6) then map.SizeX = 6 end
		
		if (map.SizeY ~= 6) then map.SizeY = 6 end

		if ((map.SizeX == 6) and (map.SizeY == 6)) then

		map.Mapdata = {
                   		{ 00, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
                  		{ -1, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
                   		{ -1, -1, -1, -1, -1, -1 },
           		    }
	          end

   		return map
    

   end


}