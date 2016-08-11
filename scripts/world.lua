function map (fun, list)
   local rv = {}
   for i, v in pairs(list)
   do
      rv[i] = fun(v)
   end
   return rv
end

function filter (fun, list)
   local rv = {}
   for i = 1, #list
   do
      if fun(list[i])
      then
	 rv[#rv + 1] = list[i]
      end
   end

   return rv
end

function list2set (list)
   local rv = {}
   for i = 1, #list
   do
      rv[list[i]] = true
   end
   return rv
end

function jointabs (s1, s2)
   local rv = {}
   for k, v in pairs(s1)
   do
      rv[k] = v
   end
   for k, v in pairs(s2)
   do
      rv[k] = v
   end
   
   return rv
end

function copyset (s1)
   return joinsets (s1, {})
end

function joinlists (l1, l2)
   local rv = {}
   for i = 1, #l1
   do
      rv[#rv + 1] = l1[i]
   end
   for i = 1, #l2
   do
      rv[#rv + 1] = l2[i]
   end
   return rv
end
	 
function keylist (tab)
   local rv = {}
   for k, v in pairs(tab)
   do
      rv[#rv + 1] = k
   end
   return rv
end

function selectrandom (list)
   return list[math.random(#list)]
end

function ijk2id (c)
   return table.concat(map(math.tointeger, c), ",")
end

function id2ijk (c)
   for i, j, k in string.gmatch(c, "(%d*),(%d*),(%d*)")
   do
      return {0 + i, 0 + j, 0 + k}
   end
end

function ijksub (ijk1, ijk2)
   return {ijk1[1] - ijk2[1], ijk1[2] - ijk2[2], ijk1[3] - ijk2[3]}
end

function ijklen (ijk1)
   return ijk1[1] * ijk1[1] + ijk1[2] * ijk1[2] + ijk1[3] * ijk1[3]
end


-- generate initial set of tiles to be connected
DIM = 6
TILECOUNT = 4
LEVELSEP = 3.0
NODES = {}
for i = 1, DIM
do
   for j = 1, DIM
   do
      for k = 1, DIM
      do
	 local id = ijk2id{i, j, k}
	 NODES[id] = {}
	 NODES[id].ijk = {i, j, k}
	 -- NODES[id].xyz = {x = (i + j) * TILECOUNT, y = (i - j) * TILECOUNT, z = k * TILECOUNT}
	 NODES[id].xyz = {x = i * TILECOUNT, y = j * LEVELSEP, z = k * TILECOUNT}
	 -- NODES[id].xyz = {x = i * TILECOUNT + j * (TILECOUNT / 2), y = j * 3, z = k * TILECOUNT}
      end
   end
end

-- reduce some of the traffic
-- for rep = 1, 1
-- do
--    local safeijk = {math.random(DIM * 5), math.random(DIM * 5), math.random(DIM * 5)}
--    local safeid = ijk2id(safeijk)
--    for id, node in pairs(NODES)
--    do
--       local rho = 5 * DIM * 5
--       local distsq = ijklen(ijksub(safeijk, node.ijk))
--       local coeff = 1 / (rho * math.sqrt(2 * math.pi))
--       local prob = coeff * math.exp(-distsq / (2 * math.pow(rho, 2)))

--       print(prob, distsq)
--       if 200 * prob < math.random()
--       then
-- 	 NODES[id] = nil
--       end
--    end
-- end

-- print("nodes left = ", #keylist(NODES))


for nodeid, node in pairs(NODES)
do
   local ijk = id2ijk(nodeid)
   local canidates = {
      {ijk[1] + 1, ijk[2], ijk[3]}, {ijk[1] - 1, ijk[2], ijk[3]},
      {ijk[1], ijk[2] + 1, ijk[3]}, {ijk[1], ijk[2] - 1, ijk[3]},
      {ijk[1], ijk[2], ijk[3] + 1}, {ijk[1], ijk[2], ijk[3] - 1}}
   canidates = filter(function (s) return NODES[s] ~= nil end, map(ijk2id, canidates))
   node.adjset = list2set(canidates)
end

forest = {}
for srcid, src in pairs(NODES)
do
   local tree = {}
   tree.nodes = list2set({srcid})
   tree.edges = {}

   tree.valences = {}
   for dstid, _ in pairs(src.adjset)
   do
      tree.valences[dstid] = srcid
   end

   forest[#forest + 1] = tree
end

final = {nodes = {}, edges = {}}
while #forest > 0
do
   -- 1. pick a random tree in forest
   local tree1i = math.random(#forest)
   local tree1 = forest[tree1i]

   local valences = keylist(tree1.valences)
   if #valences == 0
   then
      -- 2'. merge into result
      final.nodes = jointabs(tree1.nodes, final.nodes)
      final.edges = joinlists(tree1.edges, final.edges)
      table.remove(forest, tree1i)
   else
      -- 2. turn random valence into edge
      local dst = selectrandom(valences)
      local src = tree1.valences[dst]

      -- 4. find connected tree and merge it into tree1
      for tree2i, tree2 in pairs(forest)
      do
	 if tree2.nodes[dst]
	 then
	    tree1.valences = jointabs(tree1.valences, tree2.valences)
	    tree1.nodes = jointabs(tree1.nodes, tree2.nodes)
	    tree1.edges = joinlists(tree1.edges, tree2.edges)
	    tree1.edges[#tree1.edges + 1] = list2set({src, dst})

	    for d, s in pairs(tree1.valences)
	    do
	       if tree1.nodes[d]
	       then
		  tree1.valences[d] = nil
	       end
	    end

	    table.remove(forest, tree2i)
	    break
	 end
      end
   end
end
   
-- turn final graph into a tile list
tiles = {}
slides = {}
ladders = {}
for nodeid, _ in pairs(final.nodes)
do
   tiles[#tiles + 1] = NODES[nodeid].xyz
end

for _, edgeset in pairs(final.edges)
do
   local edge = keylist(edgeset)
   local sxyz = NODES[edge[1]].xyz
   local dxyz = NODES[edge[2]].xyz
   local verticalType = math.random(4)

   if sxyz.y == dxyz.y and sxyz.z == dxyz.z
   then
      -- connect Z via a tile
      local xmin = math.min(sxyz.x, dxyz.x)
      tiles[#tiles + 1] = {x = xmin + 1, y = sxyz.y, z = sxyz.z}
      tiles[#tiles + 1] = {x = xmin + 2, y = sxyz.y, z = sxyz.z}
      tiles[#tiles + 1] = {x = xmin + 3, y = sxyz.y, z = sxyz.z}
      --tiles[#tiles + 1] = {x = xmin + 4, y = sxyz.y, z = sxyz.z}
   elseif sxyz.y == dxyz.y and sxyz.x == dxyz.x
   then
      -- connect Z via a tile
      local zmin = math.min(sxyz.z, dxyz.z)
      tiles[#tiles + 1] = {x = sxyz.x, y = sxyz.y, z = zmin + 1}
      tiles[#tiles + 1] = {x = sxyz.x, y = sxyz.y, z = zmin + 2}
      tiles[#tiles + 1] = {x = sxyz.x, y = sxyz.y, z = zmin + 3}
      --tiles[#tiles + 1] = {x = sxyz.x, y = sxyz.y, z = zmin + 4}
   elseif verticalType == 1
   then
      -- connect Y via a slide
      slidestart = {x = sxyz.x + 1, y = sxyz.y, z = sxyz.z + 2.5}
      slideend = {x = dxyz.x + 1, y = dxyz.y, z = dxyz.z + 0.5}

      if slidestart.y > slideend.y
      then
          slides[#slides + 1] = slidestart
          slides[#slides + 1] = slideend
      else
          slides[#slides + 1] = slideend
          slides[#slides + 1] = slidestart
      end

      tiles[#tiles + 1] = {x = sxyz.x + 0, y = sxyz.y, z = sxyz.z + 1}
      tiles[#tiles + 1] = {x = sxyz.x + 0, y = sxyz.y, z = sxyz.z + 2}
      tiles[#tiles + 1] = {x = sxyz.x + 0, y = sxyz.y, z = sxyz.z + 3}
      tiles[#tiles + 1] = {x = sxyz.x + 1, y = sxyz.y, z = sxyz.z + 3}
      
      tiles[#tiles + 1] = {x = dxyz.x + 1, y = dxyz.y, z = dxyz.z + 0}
   elseif verticalType == 2
   then
      -- connect Y via a slide
      slidestart = {x = sxyz.x + 2.5, y = sxyz.y, z = sxyz.z + 1}
      slideend = {x = dxyz.x + 0.5, y = dxyz.y, z = dxyz.z + 1}

      if slidestart.y > slideend.y
      then
          slides[#slides + 1] = slidestart
          slides[#slides + 1] = slideend
      else
          slides[#slides + 1] = slideend
          slides[#slides + 1] = slidestart
      end

      tiles[#tiles + 1] = {x = sxyz.x + 1, y = sxyz.y, z = sxyz.z + 0}
      tiles[#tiles + 1] = {x = sxyz.x + 2, y = sxyz.y, z = sxyz.z + 0}
      tiles[#tiles + 1] = {x = sxyz.x + 3, y = sxyz.y, z = sxyz.z + 0}
      tiles[#tiles + 1] = {x = sxyz.x + 3, y = sxyz.y, z = sxyz.z + 1}
      
      tiles[#tiles + 1] = {x = dxyz.x + 0, y = dxyz.y, z = dxyz.z + 1}
   elseif verticalType == 3
   then
      -- connect Y via a ladder
      slidestart = {x = sxyz.x + 1, y = sxyz.y, z = sxyz.z + 2.5}
      slideend = {x = dxyz.x + 1, y = dxyz.y, z = dxyz.z + 0.5}

      if slidestart.y > slideend.y
      then
          slideend.y = slideend.y - 0.5
          ladders[#ladders + 1] = slidestart
          ladders[#ladders + 1] = slideend
      else
          slidestart.y = slidestart.y - 0.5
          ladders[#ladders + 1] = slideend
          ladders[#ladders + 1] = slidestart
      end

      tiles[#tiles + 1] = {x = sxyz.x + 0, y = sxyz.y, z = sxyz.z + 1}
      tiles[#tiles + 1] = {x = sxyz.x + 0, y = sxyz.y, z = sxyz.z + 2}
      tiles[#tiles + 1] = {x = sxyz.x + 0, y = sxyz.y, z = sxyz.z + 3}
      tiles[#tiles + 1] = {x = sxyz.x + 1, y = sxyz.y, z = sxyz.z + 3}
      
      tiles[#tiles + 1] = {x = dxyz.x + 1, y = dxyz.y, z = dxyz.z + 0}
   elseif verticalType == 4
   then
      -- connect Y via a ladder
      slidestart = {x = sxyz.x + 2.5, y = sxyz.y, z = sxyz.z + 1}
      slideend = {x = dxyz.x + 0.5, y = dxyz.y, z = dxyz.z + 1}

      if slidestart.y > slideend.y
      then
          ladders[#ladders + 1] = slidestart
          ladders[#ladders + 1] = slideend
      else
          ladders[#ladders + 1] = slideend
          ladders[#ladders + 1] = slidestart
      end
      
      tiles[#tiles + 1] = {x = sxyz.x + 1, y = sxyz.y, z = sxyz.z + 0}
      tiles[#tiles + 1] = {x = sxyz.x + 2, y = sxyz.y, z = sxyz.z + 0}
      tiles[#tiles + 1] = {x = sxyz.x + 3, y = sxyz.y, z = sxyz.z + 0}
      tiles[#tiles + 1] = {x = sxyz.x + 3, y = sxyz.y, z = sxyz.z + 1}
      
      tiles[#tiles + 1] = {x = dxyz.x + 0, y = dxyz.y, z = dxyz.z + 1}
   end
end

print(#final.edges)
