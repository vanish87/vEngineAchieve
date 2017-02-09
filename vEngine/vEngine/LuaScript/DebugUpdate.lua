--this is called every update for debug

--SetGbuffer1()
--SetGbuffer2()
--SetLighting()

--SetGbufferIndex(1)

function Update (x, y)
	--io.write (string.format ("Hello from %d, %d\n", x, y))
	SetGbufferIndex (1)
	return 8;
end