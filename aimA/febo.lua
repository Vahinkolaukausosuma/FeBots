-- made by jonni, cheat engine aimbot for fallen earth last client version before shutdown

MaxDist = 3600
HealthOffset = 0x128
NPCHealthPrcntOffset = 0x134
ninety = math.pi/2
xO = 0x38
yO = 0x40
zO = 0x48

function Vector(x,y,z)
	return {x=x,y=y,z=z}
end

function getDistance(p1,p2)
	return math.sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z))
end
function getMagnitude(v)
	return math.sqrt(math.pow(v.x, 2) + math.pow(v.y, 2) + math.pow(v.z, 2))
end

function CalcAngle(src, dst)
    angle = Vector(0,0,0)
    angle.x = -math.atan2(dst.x - src.x, dst.y - src.y) / math.pi * 180.0 + 90.0
    angle.y = math.sin((dst.z - src.z) / getDistance(src, dst)) * 180.0 / math.pi
	return angle
end

function readPosition(a)
	return Vector(
		readDouble(a+xO),
		readDouble(a+yO),
		readDouble(a+zO)
	)
end

function setYaw(num)
	num = num * 10
	writeFloat(getAddress('[["Frontend.exe"+00A743A8]+20]+50'),num)
end
function setPitch(num)
	num = num * 11
	writeFloat(getAddress('[["Frontend.exe"+00A743A8]+20]+54'),num)
end
function GetTarget()
	return readInteger(getAddress("Frontend.exe+A71E10"))
end

function VectorNormalize(vec)
	mag = getMagnitude(vec)
	vec.x = vec.x / mag
	vec.y = vec.y / mag
	vec.z = vec.z / mag
	return vec
end

LocalPlayer = 0
TargetPlayer = 0
nig = 39.4
dist = 0

while true do
	if isKeyPressed(70) then
		LocalPlayer = GetTarget()
	end
	 -- if isKeyPressed(81) then
		 -- nig = nig + 0.2
		 -- print(nig,dist)
 -- end
	 -- if isKeyPressed(69) then
		 -- nig = nig - 0.2
		 -- print(nig,dist)
 -- end
	if isKeyPressed(27) then
		break
	end
	TargetPlayer = GetTarget()
	if TargetPlayer ~= LocalPlayer then
		if isKeyPressed(84) then
			yes = readInteger(TargetPlayer+NPCHealthPrcntOffset)
			if yes and yes > 0 then
				if LocalPlayer > 0 then
					AimPos = readPosition(TargetPlayer)
					AimPos.z = AimPos.z - 50
					MyPos = readPosition(LocalPlayer)
					dist = getDistance(MyPos,AimPos)
					if dist < MaxDist then

						xdvecc = Vector(MyPos.x-AimPos.x,MyPos.y-AimPos.y,MyPos.z-AimPos.z)

						xdvec = VectorNormalize(xdvecc)
						xX = xdvec.x * math.cos(ninety) - xdvec.y * math.sin(ninety)
						yY = xdvec.x * math.sin(ninety) + xdvec.y * math.cos(ninety)
						xdvec.x = xX
						xdvec.y = yY
						xdvec.x = xdvec.x * nig
						xdvec.y = xdvec.y * nig
						xdvec.z = xdvec.z * nig
						
						MyPos.x = MyPos.x + xdvec.x
						MyPos.y = MyPos.y + xdvec.y
						MyPos.z = MyPos.z + xdvec.z
						yaww = CalcAngle(MyPos,AimPos)
						setYaw(yaww.x)
						--setYaw(yaww.x+nig)
						setPitch(yaww.y)
						--print(dist)
						
					end
				end
			end
		end
	end
	sleep(8)
end