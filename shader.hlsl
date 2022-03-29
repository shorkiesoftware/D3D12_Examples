struct VSInput {
    float3 position : POSITION;
};

struct PSInput {
    float4 position : SV_POSITION;
};

struct PSOutput {
    float4 color : SV_TARGET;
};

PSInput VSMain(VSInput input){
    PSInput output;
    output.position = float4(input.position, 1);
    return output;
}

PSOutput PSMain(PSInput input){
    PSOutput output;
    output.color = float4(1, 0, 0, 1);
    return output;
}

