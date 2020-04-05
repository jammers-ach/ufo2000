AddXcomTerrain {
	Name = "Siberia",
	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(xcom)/terrain/polar.*",
		"$(xcom)/terrain/xbase1.*",
		"$(xcom)/terrain/barn.*"
	},
	Maps = {
		"$(extension)/siberia00.map",
		"$(extension)/siberia01.map",
		"$(extension)/siberia02.map",
		"$(extension)/siberia03.map",
		"$(extension)/siberia04.map",
		"$(extension)/siberia05.map",
		"$(extension)/siberia06.map",
		"$(extension)/siberia07.map",
		"$(extension)/siberia08.map",
		"$(extension)/siberia09.map",
		"$(extension)/siberia10.map",
		"$(extension)/siberia11.map",
		"$(extension)/siberia12.map",
		"$(extension)/siberia13.map",
		"$(extension)/siberia14.map",
		"$(extension)/siberia15.map",
		"$(extension)/siberia16.map",
		"$(extension)/siberia17.map",
		"$(extension)/siberia18.map",
		"$(extension)/siberia19.map",
		"$(extension)/siberia20.map",
		"$(extension)/siberia21.map",
		"$(extension)/siberia22.map",
		"$(extension)/siberia23.map",
		"$(extension)/siberia24.map",
		"$(extension)/siberia25.map",
		"$(extension)/siberia26.map",
		"$(extension)/siberia27.map",
		"$(extension)/siberia28.map",
		"$(extension)/siberia29.map",
		"$(extension)/siberia30.map",
		"$(extension)/siberia31.map",
		"$(extension)/siberia32.map",
		"$(extension)/siberia33.map",
		"$(extension)/siberia34.map",
		"$(extension)/siberia35.map",
		"$(extension)/siberia36.map",
		"$(extension)/siberia37.map",
		"$(extension)/siberia38.map",
		"$(extension)/siberia39.map",
		"$(extension)/siberia40.map",
		"$(extension)/siberia41.map",
		"$(extension)/siberia42.map",
		"$(extension)/siberia43.map",
		"$(extension)/siberia44.map"
	},
MapGenerator = function(tmp)
		local function add_roads(size_x, size_y, map)
			local x, y, w

			if (math.random(1, size_y) ~= 1) then
				w = math.random(1, size_y)
				for i = 1, size_x do map[i][w] = random {27, 28, 29, 30, 31, 				32} end
			end
	
			if (math.random(1, size_x) ~= 1) then
				x = math.random(1, size_x)
				for i = 1, size_y do map[x][i] =  random {00, 01, 02, 03, 					04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14} end
			end
	
			if (math.random(1, size_y) ~= 1) then
				y = math.random(1, size_y)
				for i = 1, size_x do map[i][y] = random {15, 16, 17, 18, 19, 				20, 21, 22, 23} end
			end

	
			if (x and y) then
				map[x][y] =  random {24, 25, 26}

			end

		end

		local function random_normal()
			return random {33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44}
		end	


		for i = 1, tmp.SizeY do
			for j = 1, tmp.SizeX do
				tmp.Mapdata[i][j] = random_normal()
			end
		end

		add_roads(tmp.SizeX, tmp.SizeY, tmp.Mapdata)


		return tmp
	end		
}
