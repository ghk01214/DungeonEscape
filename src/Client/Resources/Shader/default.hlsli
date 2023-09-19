
cbuffer TRANSFORM_PARAMS : register(b0)
{
    row_major matrix matWVP;    // directX?癒?퐨 row_major 獄쎻뫗???곗쨮 ??곗졊????삼펷??獄쏆꼶?? shader?癒?퐨 column_major嚥???곗졊????삼펷????뉖┛ ????????쒑쳸?밸뻼??筌띿쉸?졽틠?⑤┛ ?袁る퉸 ???껃칰??臾믨쉐??뺣뼄.
};

cbuffer MATERIAL_PARAMS : register(b1)
{
    int int_0;
    int int_1;
    int int_2;
    int int_3;
    int int_4;

    float float_0;
    float float_1;
    float float_2;
    float float_3;
    float float_4;
};

Texture2D tex_0 : register(t0);
Texture2D tex_1 : register(t1);
Texture2D tex_2 : register(t2);
Texture2D tex_3 : register(t3);
Texture2D tex_4 : register(t4);

SamplerState sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), matWVP);   // ?ル슦紐당몴??곕뗄???띾┛ ?袁る퉸 float4, w = 1.f 嚥??類ㅼ삢, 獄쎻뫚堉?쭕????????됱젟????w = 0.f嚥??類ㅼ삢
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);
    return color;
}