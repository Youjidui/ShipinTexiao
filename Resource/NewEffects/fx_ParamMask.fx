float4	 g_fParamMask_CenterEdge;		// [0,1]
float4	 g_fParamMask_BoolMinMaxScaleCenterInv;

#define EPSILON 0.00001f

void ProcessParamMask( inout float value, 
						float2	 fCoord )
{
	if( all(g_fParamMask_BoolMinMaxScaleCenterInv.x) )
	{
		value = (value-g_fParamMask_BoolMinMaxScaleCenterInv.y) / 
		(g_fParamMask_BoolMinMaxScaleCenterInv.z-g_fParamMask_BoolMinMaxScaleCenterInv.y);
		
		float gamma			 = dot(fCoord-g_fParamMask_CenterEdge.xy,
									fCoord-g_fParamMask_CenterEdge.xy);
		float gammaUnit	 = dot(g_fParamMask_CenterEdge.zw-g_fParamMask_CenterEdge.xy,
									g_fParamMask_CenterEdge.zw-g_fParamMask_CenterEdge.xy);
		gamma = lerp(g_fParamMask_BoolMinMaxScaleCenterInv.w,1.f,gamma/gammaUnit);	
		value = pow(value+EPSILON,gamma);
		
		value = value * (g_fParamMask_BoolMinMaxScaleCenterInv.z-g_fParamMask_BoolMinMaxScaleCenterInv.y)
					+ g_fParamMask_BoolMinMaxScaleCenterInv.y;
					
	}
}

void ProcessParam2Mask( inout float2 value, 
						float2	 fCoord )
{
	if( all(g_fParamMask_BoolMinMaxScaleCenterInv.x) )
	{
		value = (value-g_fParamMask_BoolMinMaxScaleCenterInv.y) / 
		(g_fParamMask_BoolMinMaxScaleCenterInv.z-g_fParamMask_BoolMinMaxScaleCenterInv.y);
		
		float gamma			 = dot(fCoord-g_fParamMask_CenterEdge.xy,
									fCoord-g_fParamMask_CenterEdge.xy);
		float gammaUnit	 = dot(g_fParamMask_CenterEdge.zw-g_fParamMask_CenterEdge.xy,
									g_fParamMask_CenterEdge.zw-g_fParamMask_CenterEdge.xy);
		gamma = lerp(g_fParamMask_BoolMinMaxScaleCenterInv.w,1.f,gamma/gammaUnit);	
		value = pow(value+EPSILON,gamma);
		
		value = value * (g_fParamMask_BoolMinMaxScaleCenterInv.z-g_fParamMask_BoolMinMaxScaleCenterInv.y)
					+ g_fParamMask_BoolMinMaxScaleCenterInv.y;
					
	}
}

void ProcessCoordMask( inout float2 coordInOut )
{
	if( all(g_fParamMask_BoolMinMaxScaleCenterInv.x) )
	{
	
		// 变换坐标系中心
		float2 fOffsetCenter = coordInOut - g_fParamMask_CenterEdge.xy;
		
		// 归一化坐标
		float2 fValue = fOffsetCenter / (g_fParamMask_CenterEdge.zw-g_fParamMask_CenterEdge.xy);
		
		// 做gamma映射
		float2 fValueChanged = pow(abs(fValue),1.f/g_fParamMask_BoolMinMaxScaleCenterInv.w);
		fValueChanged *= sign(fOffsetCenter);
		
		// 恢复坐标系
		coordInOut = fValueChanged * (g_fParamMask_CenterEdge.zw-g_fParamMask_CenterEdge.xy) + g_fParamMask_CenterEdge.xy;			
	}
}
						
