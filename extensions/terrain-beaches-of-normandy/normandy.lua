--------------------------------------------------------------------------
--  BEACHES OF NORMANDY, c 2005 by Violazr			V 0.91	--
--									--
--  With generation code for a terrain of randomized columns.		--
--  See comments and http://www.lua.org/pil/index.htm for explanations.	--
--------------------------------------------------------------------------



AddXcomTerrain {				-- self-explanatory --

	Name = "Beaches Of Normandy",

	Tiles =	{
		"$(xcom)/terrain/blanks.*",
		"$(extension)/BON1.*",
		"$(extension)/BON2.*",
		"$(extension)/BON3.*",
		"$(extension)/BON4.*",
	},

	Maps = {
		"$(extension)/bon_51.map",
		"$(extension)/bon_52.map",
		"$(extension)/bon_53.map",
		"$(extension)/bon_54.map",
		"$(extension)/bon_61.map",
		"$(extension)/bon_62.map",
		"$(extension)/bon_63.map",
		"$(extension)/bon_64.map",
		"$(extension)/bon_65.map",
		"$(extension)/bon_66.map",
		"$(extension)/bon_71.map",
		"$(extension)/bon_72.map",
		"$(extension)/bon_73.map",
		"$(extension)/bon_74.map",
		"$(extension)/bon_81.map",
		"$(extension)/bon_82.map",
		"$(extension)/bon_83.map",
		"$(extension)/bon_84.map",
		"$(extension)/bon_91.map",
		"$(extension)/bon_92.map",
		"$(extension)/bon_93.map",
		"$(extension)/bon_94.map",
},


	MapGenerator = function(map)	-- excess matrix elements will be ignored --

		map.Mapdata = {
                	   		{ -1, -1, -1, -1, -1, -1 },
                   			{ -1, -1, -1, -1, -1, -1 },
                   			{ -1, -1, -1, -1, -1, -1 },
         	         		{ -1, -1, -1, -1, -1, -1 },
                	   		{ -1, -1, -1, -1, -1, -1 },
                   			{ -1, -1, -1, -1, -1, -1 },
           			}


	local p5, p6, p7, p8, p9	-- prevent interference with global vars --


		if ((map.SizeX == 4) and (map.SizeY == 4)) then

			p5 = 1 		-- x postitions of the map columns --
			p6 = 7
			p7 = 2
			p8 = 3
			p9 = 7
	


		elseif ((map.SizeX == 5) and (map.SizeY == 5)) then
	
			if (math.random(0, 1) == 1) then	-- different setups --

				p5 = 1
				p6 = 2
				p7 = 3
				p8 = 4
				p9 = 7
			else
				p5 = 1
				p6 = 7
				p7 = 2
				p8 = 3
				p9 = 5
			end
	


		else		-- 6x6 optimal size, full setup --

			p5 = 1
			p6 = 2
			p7 = 3
			p8 = 4
			p9 = 6
	
		end



				-- column randomizers --

		local function random_5()
			return random {51, 52, 53, 54}
		end	

		for i = 1, map.SizeY, 2 do	-- steps of 2 for double maps --
			map.Mapdata[i][p5] = random_5()
		end


		local function random_6()
			return random {61, 62, 63, 64, 65, 66}
		end	

		for i = 1, map.SizeY do
			map.Mapdata[i][p6] = random_6()
		end


		local function random_7()
			return random {71, 72, 73, 74}
		end	

		for i = 1, map.SizeY, 2 do
			map.Mapdata[i][p7] = random_7()
		end


		local function random_8()
			return random {81, 82, 83, 84}
		end	

		for i = 1, map.SizeY, 2 do
			map.Mapdata[i][p8] = random_8()
		end


		local function random_9()
			return random {91, 92, 93, 94}
		end	

		for i = 1, map.SizeY, 2 do
			map.Mapdata[i][p9] = random_9()
		end




				-- make pairs less likely. use "while" to disable. --

		for i = 1, 3, 2 do

			if (map.Mapdata[i][p5] == map.Mapdata[i+2][p5])
			then map.Mapdata[i+2][p5] = random_5()
			end

			if (map.Mapdata[i][p7] == map.Mapdata[i+2][p7])
			then map.Mapdata[i+2][p7] = random_7()
			end

			if (map.Mapdata[i][p8] == map.Mapdata[i+2][p8])
			then map.Mapdata[i+2][p8] = random_8()
			end

			if (map.Mapdata[i][p9] == map.Mapdata[i+2][p9])
			then map.Mapdata[i+2][p9] = random_9()
			end
		end


		for i = 1, (map.SizeY - 1) do
			if (map.Mapdata[i][p6] == map.Mapdata[i+1][p6])
			then map.Mapdata[i+1][p6] = random_6()
			end
		end




				--  exchange badly cut-off maps in 5x5, no pairs --

		if ((map.SizeX == 5) and (map.SizeY == 5)) then

			while map.Mapdata[5][p5] == 52
			or map.Mapdata[5][p5] == 53
			or map.Mapdata[5][p5] == map.Mapdata[3][p5]
			do
				map.Mapdata[5][p5] = random_5()
			end

			while map.Mapdata[5][p7] == 72
			or map.Mapdata[5][p7] == map.Mapdata[3][p7]
			do
				map.Mapdata[5][p7] = random_7()
			end

			while map.Mapdata[5][p8] == 81
			or map.Mapdata[5][p8] == 82
			or map.Mapdata[5][p8] == map.Mapdata[3][p8]
			do
				map.Mapdata[5][p8] = random_8()
			end

			while map.Mapdata[5][p9] == 91
			or map.Mapdata[5][p9] == 94
			or map.Mapdata[5][p9] == map.Mapdata[3][p9]
			do
				map.Mapdata[5][p9] = random_9()
			end

		end



				-- rare special events --

			local event = (math.random(0, 1000))

				if event == 42 then
	
					map.SizeX = 6
					map.SizeY = 6
	
					map.Mapdata = {
                		   		{ -1, 62, 62, 62, 62, -1},
                		   		{ 62, -1, 62, -1, 62, 62},
                	   			{ 62, 62, 62, 62, 62, 62},
                	   			{ 62, -1, 62, 62, -1, 62},
     	        	   	   		{ 62, 62, -1, -1, 62, 62},
        	        	   		{ -1, 62, 62, 62, 62, -1},
           					}
				end


				if event < 10 then
					for i = 1, map.SizeY do
						for j = 1, map.SizeX do
							map.Mapdata[i][j] = random_6()
						end
					end
				end





   	return map	-- send map data to game --
    

   end


}


			-- Thanks for reading! ;) --
