// pages fbo
uniform sampler2D Texture0;
uniform sampler2D Texture1;

// result fbo
uniform sampler2D Texture2;

// result fbo blurred
uniform sampler2D Texture3;


uniform float cameraZoom;
uniform vec2 bufferDim;
uniform vec2 resolution;
uniform vec3 cameraPos;
uniform vec4 fogPos;





varying vec2 TexCoord0;

$

void main() {

    TexCoord0 = gl_MultiTexCoord0.xy;
    gl_Position = gl_Vertex;
}

$


float unpack16(vec2 num) {
    return num.r*255.0 + num.g*65280.0;
}

vec3 getFogColor(float lv) {
    float newlv = clamp( 1.0-pow( (lv-0.5)*2.0, 2.0 ), 0.0, 1.0); 
    vec3 fogColor1 = vec3(0.05, 0.1, 0.3);
    vec3 fogColor2 = vec3(0.7, 0.8, 1.0);

    float newlv2 = clamp( 1.0-pow( (lv-0.5)*4.0 - 1.0, 2.0 ), 0.0, 1.0); 

    return mix(fogColor1,fogColor2, newlv ) + pow(newlv2,4.0)/4.0;
}

void main() {

    vec4 tex0 = texture2D(Texture0, TexCoord0.xy);
    vec4 tex1 = texture2D(Texture1, TexCoord0.xy);

    vec4 tex2 = texture2D(Texture2, TexCoord0.xy);
    vec4 tex3 = texture2D(Texture3, TexCoord0.xy);

    
    float newZoom = min(cameraZoom,1.0);
    float baseHeight = unpack16(tex0.rg);



    vec3 worldPosition = vec3(0.0,0.0,0.0);
    vec2 tcMod = (vec2(TexCoord0.x,1.0-TexCoord0.y)*2.0-1.0 );
    tcMod.x *= bufferDim.x/(newZoom);
    tcMod.y *= bufferDim.y/(newZoom);


    tcMod.y -= cameraPos.z;
    worldPosition.x = tcMod.y + tcMod.x/2.0 + (baseHeight);
    worldPosition.y = tcMod.y - tcMod.x/2.0 + (baseHeight);
    worldPosition.z = baseHeight;
    worldPosition.x += cameraPos.x;
    worldPosition.y += cameraPos.y;



    /////////////

    vec3 newFog = vec3(0.0);
    vec3 fogXYZ = vec3(0.0);
    float fogLen = 0.0;
    float hfog = 1.0;
    


    newFog = (fogPos.xyz-worldPosition.xyz);
    newFog /= 1024.0;
    newFog.xy /= 2.0;
    fogXYZ = 1.0-clamp( newFog, 0.0, 1.0);
    fogLen = 1.0-clamp(1.0-(fogXYZ.x*fogXYZ.y),0.0,1.0);
    hfog = min(clamp(sqrt(fogLen),0.0,1.0),fogXYZ.z);
    hfog *= float(baseHeight > 0.0);
    //hfog = clamp(hfog+clamp(1.0-waveh,0.0,1.0)/2.0,0.0,1.0);
    hfog = pow( hfog , 2.0);
    hfog = 1.0-clamp(hfog,0.0,1.0);


    /////////////

    vec3 fogColor = getFogColor(TexCoord0.y);

    vec3 finalCol = mix(tex2.rgb,tex3.rgb,hfog);

    finalCol = mix(finalCol,fogColor, hfog);

    gl_FragData[0] = vec4(finalCol,1.0);

}