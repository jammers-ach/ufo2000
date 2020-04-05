AddXcomTerrain {
	Name = "Mothership",
	Tiles =	{
		"$(xcom)/terrain/BLANKS.*",
		"$(xcom)/terrain/BLANKS.*",
		"$(xcom)/terrain/U_EXT02.*",
		"$(xcom)/terrain/U_WALL02.*",
		"$(xcom)/terrain/U_DISEC2.*",
		"$(xcom)/terrain/U_BITS.*",
		"$(xcom)/terrain/U_OPER2.*",
		"$(xcom)/terrain/U_PODS.*",
		"$(xcom)/terrain/POLAR.*"
	},
	Maps = {
		"$(extension)/mothership00.map"
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