#version 420

/***************In*******************/

/*From Vertex Shader*/
in vec4 fColour;	
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

/***************Pass1*******************/

/*Base Textures*/
uniform sampler2D textSamp00;
uniform sampler2D textSamp01;
//uniform sampler2D textSamp02;
//uniform sampler2D textSamp03;
//uniform sampler2D textSamp04;
//uniform sampler2D textSamp05;
//uniform sampler2D textSamp06;
//uniform sampler2D textSamp07;

uniform vec2 tex_0_1_ratio;
//uniform vec4 tex_0_3_ratio;		// x = 0, y = 1, z = 2, w = 3
//uniform vec4 tex_4_7_ratio;

// Apparently, you can now load samplers into arrays, 
// instead of using the sample2DArray sampler;
// uniform sampler2D textures[10]; 	// for instance

/*Skybox Textures*/
uniform samplerCube skyBox1;
uniform samplerCube skyBox2;
uniform bool bIsSkyBox;

/*Transparent*/
uniform vec4 diffuseColour;				// use a for transparency

/*Imposter*/
// If true, then:
// - don't light
// - texture map
// - Use colour to compare to black and change alpha 
// - Use colour to compare the black for discard
//uniform bool bIsImposter;
uniform bool bDiscard; // Used to discard stuff (remove or use in imposter)
uniform sampler2D discardTexture; // Shold be removed

/*For Instnced objects*/
uniform bool bInstance;

/***************Pass2*******************/

/*Lights?*/
uniform vec4 specularColour;

/*Lights*/
struct sLight
{
	vec4 position;
	vec4 diffuse;
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
					// 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 20;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 140 uniforms


/*Used to draw debug (or unlit) objects*/
uniform vec4 debugColour;			
uniform bool bDoNotLight;

/*Useed for Lights and Reflection/Refraction*/
uniform vec4 eyeLocation;

/***************Pass3*******************/

/*Reflection/Refraction*/
uniform samplerCube refCube;
uniform bool bIsReflective;
uniform bool bIsRefractive;
uniform float reflectionTextureRatio;
uniform float refractionTextureRatio;
uniform float refractionStrength;

/*Effects and FBO*/
uniform sampler2D fboTexture;

uniform float screenWidth;
uniform float screenHeight;
uniform bool bIsFullScreenEffect;

uniform sampler2D overlayEffectTexture;
uniform bool bUseOverlayEffect;

uniform bool bColourEffect;
uniform vec4 colourEffectVal;

uniform bool bBlurEffect;
uniform float blurEffectValue;

uniform bool bSwirlEffect;
uniform float swirlEffectValue;

uniform int passNumber; // replace with bool or remove


/***************Out*******************/

out vec4 pixelColour;			// GL_COLOR_ATTACHMENT0


// Often grouped by “usage”: Per scene, Per frame, Per object
// Used to replace glUniform and glGetUnifromLocation with glActiveUniforms in C++ code
// Here's an example of the NUB
//layout(std140) uniform myBlockType
//{
//	vec2 SomeData;
//	vec3 MoreData;
//	//	bool isSkybox;
//} NUB_setPerScene;


/***************Functions*******************/

vec4 calcualteLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular);


vec2 fullScreenEffect(vec4 fragCoord, vec2 screenSize);

vec2 swirlEffect(sampler2D baseTexture, vec2 uv, float angleValue);


vec4 blurEffect(sampler2D baseTexture, vec2 uv, float blurOffset);


vec4 colourEffect(vec4 baseColour, vec4 value);

vec4 overlayEffect(vec4 baseColour, sampler2D overlayTexture, vec2 uv);

void main() {

	// It should be used to be detected by glActiveUniforms
	//if (NUB_setPerScene.SomeData.x > 0.5f)
	//{
	//	pixelColour.g = 1.0f;
	//	pixelColour.rb = vec2(0.0f, 0.0f);
	//}

	if (passNumber == 1)
	{
		vec2 uv = fUVx2.st;

		if (bIsFullScreenEffect)
		{
			uv = fullScreenEffect(gl_FragCoord, vec2(screenWidth, screenHeight));
		}

		if (bSwirlEffect)
		{
			uv = swirlEffect(fboTexture, uv, swirlEffectValue);
		}


		// No effect
		pixelColour = texture(fboTexture, uv);

		if (bBlurEffect)
		{
			pixelColour = blurEffect(fboTexture, uv, blurEffectValue);
		}


		if (bColourEffect)
		{
			pixelColour = colourEffect(pixelColour, colourEffectVal);
		}

		if (bUseOverlayEffect)
		{
			vec2 old_uv = fUVx2.st;
			pixelColour = overlayEffect(pixelColour, overlayEffectTexture, old_uv);
		}

		return;
	}




	// Shader Type #1  	
	if ( bDoNotLight )
	{
		pixelColour.rgb = debugColour.rgb;
		pixelColour.a = 1.0f;				// NOT transparent
		return;
	}



//	if ( bIsImposter )
//	{
//		// If true, then:
//		// - don't light
//		// - texture map
//		// - Use colour to compare to black and change alpha 
//		// - Use colour to compare the black for discard
//		vec3 texRGB = texture( textSamp00, fUVx2.st ).rgb;
//		
//		pixelColour.rgb = texRGB.rgb;
//		
//		// Note that your eye doesn't see this, 
//		// Use this equation instead: 0.21 R + 0.72 G + 0.07 B
//		float grey = (texRGB.r + texRGB.g + texRGB.b)/3.0f;
//		
//		// If it's REALLY black, then discard
//		if ( grey < 0.05 ) 	{	discard; }
//		
//		// Otherwise control alpha with "black and white" amount
//		pixelColour.a = grey;
//		if ( pixelColour.a < diffuseColour.a )
//		{
//			pixelColour.a = diffuseColour.a;
//		}
//		
//		//pixelColour.a = diffuseColour.a;
//		return;
//	}

	

	if ( bIsSkyBox )
	{
		// I sample the skybox using the normal from the surface
		vec3 tex0_RGB = texture( skyBox1, fNormal.xyz ).rgb;
		vec3 tex1_RGB = texture( skyBox2, fNormal.xyz ).rgb;

		vec3 texRGB =   ( tex_0_1_ratio.x * tex0_RGB ) 
					  + ( tex_0_1_ratio.y * tex1_RGB );

		pixelColour.rgb = texRGB.rgb;
		pixelColour.a = 1.0f;				// NOT transparent
		return;
	}

	if (bIsReflective)
	{
		vec4 materialColour = diffuseColour;

		vec3 tex0_RGB = texture(textSamp00, fUVx2.st).rgb;
		vec3 tex1_RGB = texture(textSamp01, fUVx2.st).rgb;
		//	vec3 tex2_RGB = texture( textSamp02, fUVx2.st ).rgb;
		//	vec3 tex3_RGB = texture( textSamp03, fUVx2.st ).rgb;
		//	
		//	vec3 texRGB =   ( tex_0_3_ratio.x * tex0_RGB ) 
		//				  + ( tex_0_3_ratio.y * tex1_RGB );
		//				  + ( tex_0_3_ratio.z * tex2_RGB )
		//				  + ( tex_0_3_ratio.w * tex3_RGB );
		vec3 texRGB = (tex_0_1_ratio.x * tex0_RGB)
			+ (tex_0_1_ratio.y * tex1_RGB);

		vec4 outColour;

		outColour.rgb = texRGB;

		if (bInstance)
		{
			outColour.rgb = texRGB;
		}
		else
		{
			outColour = calcualteLightContrib(texRGB.rgb, fNormal.xyz, fVertWorldLocation.xyz, specularColour);
		}




		//normalize the normal
		vec3 N = normalize(fNormal.xyz);

		//get the normalized view vector from the object space vertex 
		//position and object space camera position
		vec3 V = normalize(fVertWorldLocation.xyz - eyeLocation.xyz); // normalize(Position - cameraPos)

		vec3 reflectVector = reflect(V, N);
		vec4 reflectColour = texture(refCube, reflectVector);

		pixelColour = reflectColour * reflectionTextureRatio + outColour * (1.0f - reflectionTextureRatio);

		return;
	}


	if (bIsRefractive)
	{
		vec4 materialColour = diffuseColour;

		vec3 tex0_RGB = texture(textSamp00, fUVx2.st).rgb;
		vec3 tex1_RGB = texture(textSamp01, fUVx2.st).rgb;
		//	vec3 tex2_RGB = texture( textSamp02, fUVx2.st ).rgb;
		//	vec3 tex3_RGB = texture( textSamp03, fUVx2.st ).rgb;
		//	
		//	vec3 texRGB =   ( tex_0_3_ratio.x * tex0_RGB ) 
		//				  + ( tex_0_3_ratio.y * tex1_RGB );
		//				  + ( tex_0_3_ratio.z * tex2_RGB )
		//				  + ( tex_0_3_ratio.w * tex3_RGB );
		vec3 texRGB = (tex_0_1_ratio.x * tex0_RGB)
			+ (tex_0_1_ratio.y * tex1_RGB);

		vec4 outColour;

		outColour.rgb = texRGB;

		if (bInstance)
		{
			outColour.rgb = texRGB;
		}
		else
		{
			outColour = calcualteLightContrib(texRGB.rgb, fNormal.xyz, fVertWorldLocation.xyz, specularColour);
		}




		//normalize the normal
		vec3 N = normalize(fNormal.xyz);

		//get the normalized view vector from the object space vertex 
		//position and object space camera position
		vec3 V = normalize(fVertWorldLocation.xyz - eyeLocation.xyz); // normalize(Position - cameraPos)

		//Air		1.0
		//Water		1.33
		//Ice		1.309
		//Glass		1.52
		//Diamond	2.42
		float ratio = 1.0f / refractionStrength;
		vec3 refractVector = refract(V, N, ratio);
		vec4 refractColour = texture(refCube, refractVector);

		pixelColour = refractColour * refractionTextureRatio + outColour * (1.0f - refractionTextureRatio);

		return;
	}


	// Shader Type #2
	vec4 materialColour = diffuseColour;
	
	vec3 tex0_RGB = texture( textSamp00, fUVx2.st ).rgb;
	vec3 tex1_RGB = texture( textSamp01, fUVx2.st ).rgb;
//	vec3 tex2_RGB = texture( textSamp02, fUVx2.st ).rgb;
//	vec3 tex3_RGB = texture( textSamp03, fUVx2.st ).rgb;
//	
//	vec3 texRGB =   ( tex_0_3_ratio.x * tex0_RGB ) 
//				  + ( tex_0_3_ratio.y * tex1_RGB );
//				  + ( tex_0_3_ratio.z * tex2_RGB )
//				  + ( tex_0_3_ratio.w * tex3_RGB );
	vec3 texRGB =   ( tex_0_1_ratio.x * tex0_RGB ) 
				  + ( tex_0_1_ratio.y * tex1_RGB );
	
	vec4 outColour;

	outColour.rgb = texRGB;

	if ( bInstance )
	{
		outColour.rgb = texRGB;
	}
	else
	{
		outColour = calcualteLightContrib( texRGB.rgb, fNormal.xyz, fVertWorldLocation.xyz, specularColour );
	}



	pixelColour.rgb = outColour.rgb;
	
	// Set the "a" of diffuse to set the transparency
	pixelColour.a = diffuseColour.a; 		// "a" for alpha, same as "w"
	
	if ( bDiscard ) {
		// Discard
		vec3 texDiscard_RGB = texture( discardTexture, fUVx2.st ).rgb;
		float grey = (texDiscard_RGB.r + texDiscard_RGB.g + texDiscard_RGB.b)/3.0f;
		if ( grey < 0.05f )		// Basically "black"
		{
			discard;
		}

		// Otherwise control alpha with "black and white" amount
		pixelColour.a = grey;
		if ( pixelColour.a < diffuseColour.a )
		{
			pixelColour.a = diffuseColour.a;
		}
	}
	

	// If too dim
	//// Projector is too dim
	//pixelColour.rgb *= 1.8f;

	// Depth Example
	//// clear the colour
	//pixelColour.rgb *= 0.001;

	//// maybe take from main
	//float farPlane = 1000.0f;
	//float nearPlane = 1.0f;

	//// Back to Normalized Device Coordinates
	//float z_ndc = gl_FragCoord.z * 2.0f - 1.0f;
	//// From NDC to range of near to far
	//float depth = (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - z_ndc * (farPlane - nearPlane));
	//depth /= farPlane;

	//pixelColour = vec4(vec3(depth), 1.0f);

}	



vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
			lightContrib *= dotProduct;		
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		// -1 to 1
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		// If it's negative, will clamp to 0 --- range from 0 to 1
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight ); 
		
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{
			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}		
		}
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );
	}
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}


vec2 fullScreenEffect(vec4 fragCoord, vec2 screenSize)
{
	//UV.s = fVertWorldLocation.x / 25.0f;
	//UV.t = fVertWorldLocation.y / 25.0f;

	return vec2(fragCoord.x / float(screenSize.x),	// "u" and "Width"
				fragCoord.y / float(screenSize.y));	// "v" or "Height"
}

vec2 swirlEffect(sampler2D baseTexture, vec2 uv, float angleValue)
{
	vec2 texSize = textureSize(baseTexture, 0);

	// Swirl effect parameters (all can be set outside, but we are setting only angle)
	vec2 center = vec2(texSize.s / 2.0f, texSize.t / 2.0f); // Center of swirl is at the center of texture
	float radius = (texSize.s < texSize.t) ? texSize.s / 2.0f : texSize.t / 2.0f; // Uses half of width or height (whatever smaller)
	float angle = angleValue; // 0.8f

	vec2 uvSwirl = uv * texSize;
	uvSwirl -= center;
	float dist = length(uvSwirl);
	if (dist < radius)
	{
		float percent = (radius - dist) / radius;
		float theta = percent * percent * angle * 8.0f;
		uvSwirl = vec2(dot(uvSwirl, vec2(cos(theta), -sin(theta))), dot(uvSwirl, vec2(sin(theta), cos(theta))));
	}
	uvSwirl += center;
	uvSwirl /= texSize;

	return uvSwirl;
}

vec4 blurEffect(sampler2D baseTexture, vec2 uv, float blurOffset)
{
	//float bo = 0.0025f;		// For "blur offset"

	vec4 baseColour1 = texture(baseTexture, vec2(uv.s + 0.0f,		uv.t + 0.0f));
	vec4 baseColour2 = texture(baseTexture, vec2(uv.s - blurOffset, uv.t + 0.0f));
	vec4 baseColour3 = texture(baseTexture, vec2(uv.s + blurOffset, uv.t + 0.0f));
	vec4 baseColour4 = texture(baseTexture, vec2(uv.s + 0.0f,		uv.t - blurOffset));
	vec4 baseColour5 = texture(baseTexture, vec2(uv.s + 0.0f,		uv.t + blurOffset));

	return	0.5f	* baseColour1 +
			0.125f	* baseColour2 +
			0.125f	* baseColour3 +
			0.125f	* baseColour4 +
			0.125f	* baseColour5;
}


vec4 colourEffect(vec4 baseColour, vec4 value)
{
	return vec4(baseColour * value);
}

vec4 overlayEffect(vec4 baseColour, sampler2D overlayTexture, vec2 uv)
{
	vec4 ovelayColour = texture(overlayTexture, uv);

	return vec4 (baseColour.rgb * (1.0f - ovelayColour.a) + ovelayColour.rgb * ovelayColour.a, baseColour.a);
}