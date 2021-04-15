#version 420

//in vec4 vColour;				// Was vec3
layout(location = 0) in vec4 vPosition;				// Was vec3
layout(location = 1) in vec4 vNormal;				// Vertex normal
layout(location = 2) in vec4 vUVx2;					// 2 x Texture coords

in vec4 vTangent;				// For bump mapping
in vec4 vBiNormal;				// For bump mapping
layout(location = 3) in vec4 vBoneID;				// For skinned mesh (FBX)
layout(location = 4) in vec4 vBoneWeight;			// For skinned mesh (FBX)

layout(location = 5) in mat4 inst_matModel;				// Model or World 

//uniform mat4 MVP;
uniform mat4 matModel;					// Model or World
uniform mat4 matModelInverseTranspose;	// For normal calculation
uniform mat4 matView; 					// View or camera
uniform mat4 matProj;					// Projection transform

uniform bool bInstance;		// Mine

uniform sampler2D heightMap;
uniform bool useHeightMap;	// If true, use heightmap

const int MAXNUMBEROFBONES = 100;
uniform mat4 matBonesArray[MAXNUMBEROFBONES];
uniform bool isSkinnedMesh;

uniform vec2 textOffset;
uniform bool useMovingTexture;

//out vec4 fColour;
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;

void main() {

    vec4 vertPosition = vPosition;

	if (isSkinnedMesh)
	{
		// *************************************************
		mat4 BoneTransform = matBonesArray[ int(vBoneID[0]) ] * vBoneWeight[0];
		     BoneTransform += matBonesArray[ int(vBoneID[1]) ] * vBoneWeight[1];
		     BoneTransform += matBonesArray[ int(vBoneID[2]) ] * vBoneWeight[2];
		     BoneTransform += matBonesArray[ int(vBoneID[3]) ] * vBoneWeight[3];

		// Apply the bone transform to the vertex:
		vec4 vertOriginal = vec4(vertPosition.xyz, 1.0f);

		vec4 vertAfterBoneTransform = BoneTransform * vertOriginal;

		mat4 matMVP = matProj * matView * matModel;		
		// Transform the updated vertex location (from the bone)
		//  and transform with model view projection matrix (as usual)
		gl_Position = matMVP * vertAfterBoneTransform;


		// Then we do the normals, etc.
		fVertWorldLocation = matModel * vertAfterBoneTransform;	

		// Updated "world" or "model" transform 
		mat4 matModelAndBone = matModel * BoneTransform;

		vec3 theNormal = normalize(vNormal.xyz);
		fNormal = inverse(transpose(matModelAndBone)) * vec4(theNormal, 1.0f);
		fNormal.xyz = normalize(fNormal.xyz); 
		// *************************************************	
	}
	else
	{
		if ( useHeightMap )
		{
			//// Move the y value by some amount from texture	
			//// Since it's black and white, I only sample 1 colour.
			//vec2 texUV = vUVx2.st + textOffset.xy;
			//float texValue = texture( heightMap, texUV.st ).r;
			//float ratio = 1.0f;
			//vertPosition.y += (texValue * ratio);


			// Move the y value by some amount from texture	
			// Since it's black and white, I only sample 1 colour.
			vec2 texUV1 = vUVx2.st + textOffset.xy;
			// This will pick a completely different location
			// (note the reversal of the xy to yx, called a "swizzle")
			vec2 texUV2 = vUVx2.st + textOffset.yx * vec2(-0.5f, 0.75f);
			float texValue1 = texture( heightMap, texUV1.st ).r;
			float texValue2 = texture( heightMap, texUV2.st ).r;
			float ratio = 0.25f;
			float texValue = (texValue1 + texValue2) * ratio;
			vertPosition.y += (texValue * ratio);
		}

		if ( bInstance )
		{
			mat4 matMVP = matProj * matView * inst_matModel;
			gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);
	
			// Vertex location in "world space"
			fVertWorldLocation = inst_matModel * vec4(vertPosition.xyz, 1.0);
		}
		else
		{
			mat4 matMVP = matProj * matView * matModel;
			// Scale it into a cube that's -1.0 to 1.0
			gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);
	
			// Vertex location in "world space"
			fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0);
		}

		vec3 theNormal = normalize(vNormal.xyz);
 		fNormal = matModelInverseTranspose * vec4(theNormal, 1.0f);
		fNormal.xyz = normalize(fNormal.xyz); 
	
		// Pass the colour and UV unchanged.
		//fColour = vColour;
	}

	if ( useMovingTexture )
	{
		fUVx2.st = vUVx2.st + textOffset.xy;
	}
	else
	{
		fUVx2 = vUVx2;
	}
}