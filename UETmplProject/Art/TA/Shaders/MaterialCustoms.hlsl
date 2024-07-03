int iRX = floor(XY.x / RVTVSideLength);
int iRY = floor(XY.y / RVTVSideLength);
float2 v2Mod = float2(fmod(XY.x, RVTVSideLength), fmod(XY.y, RVTVSideLength));
uint4 colorData = RVTVArrayTex.Load(float3(iRX, iRY, 0));
if (colorData.r == 1) return float4(v2Mod.xy, colorData.g, 1.0);
return float4(v2Mod.xy, 0.0, 0.0);