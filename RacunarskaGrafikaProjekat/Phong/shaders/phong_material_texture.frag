#version 330 core

struct PositionalLight {
	vec3 Position;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Kc;
	float Kl;
	float Kq;
};

struct DirectionalLight {
	vec3 Position;
	vec3 Direction;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float InnerCutOff;
	float OuterCutOff;
	float Kc;
	float Kl;
	float Kq;
};

struct Material {
	//Diffuse is used as ambient as well since the light source
	// defines the ambient colour
	sampler2D Kd;
	sampler2D Ks;
	float Shininess;
};

uniform PositionalLight uPointLight;
uniform DirectionalLight uSpotlight;
uniform DirectionalLight uDirLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

uniform vec3 uPointLightPosition1;
uniform vec3 uPointLightPosition2;
uniform vec3 uPointLightPosition3;

uniform vec3 uSpotLightPosition1;
uniform vec3 uSpotLightPosition2;
uniform vec3 uSpotLightDirection1;
uniform vec3 uSpotLightDirection2;

in vec2 UV;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;

out vec4 FragColor;

void main() {
	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);
	//Directional light
	vec3 DirLightVector = normalize(-uDirLight.Direction);
	float DirDiffuse = max(dot(vWorldSpaceNormal, DirLightVector), 0.0f);
	vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
	// 32 is the specular shininess factor. Hardcoded for now
	float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 DirAmbientColor = uDirLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * vec3(texture(uMaterial.Kd, UV));
	vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * vec3(texture(uMaterial.Ks, UV));
	vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;

	// Point light1 - srednja vatra
	vec3 PtLightVector = normalize(uPointLightPosition1 - vWorldSpaceFragment);
	float PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	vec3 PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor = uPointLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 PtDiffuseColor = PtDiffuse * uPointLight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 PtSpecularColor = PtSpecular * uPointLight.Ks * vec3(texture(uMaterial.Ks, UV));

	float PtLightDistance = length(uPointLightPosition1 - vWorldSpaceFragment);
	float PtAttenuation = 1.0f / (uPointLight.Kc + uPointLight.Kl * PtLightDistance + uPointLight.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor1 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);


	// Point light2 - leva vatra
	PtLightVector = normalize(uPointLightPosition2 - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uPointLight.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uPointLight.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uPointLight.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = length(uPointLightPosition2 - vWorldSpaceFragment);
	PtAttenuation = 1.0f / (uPointLight.Kc + uPointLight.Kl * PtLightDistance + uPointLight.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor2 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	// Point light3 - desna vatra
	PtLightVector = normalize(uPointLightPosition3 - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uPointLight.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uPointLight.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uPointLight.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = length(uPointLightPosition3 - vWorldSpaceFragment);
	PtAttenuation = 1.0f / (uPointLight.Kc + uPointLight.Kl * PtLightDistance + uPointLight.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor3 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);


	// SpotLight 1
	vec3 SpotlightVector = normalize(uSpotLightPosition1 - vWorldSpaceFragment);

	float SpotDiffuse = max(dot(vWorldSpaceNormal, SpotlightVector), 0.0f);
	vec3 SpotReflectDirection = reflect(-SpotlightVector, vWorldSpaceNormal);
	float SpotSpecular = pow(max(dot(ViewDirection, SpotReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 SpotAmbientColor = uSpotlight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotDiffuseColor = SpotDiffuse * uSpotlight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotSpecularColor = SpotSpecular * uSpotlight.Ks * vec3(texture(uMaterial.Ks, UV));

	float SpotlightDistance = length(uSpotLightPosition1 - vWorldSpaceFragment);
	float SpotAttenuation = 1.0f / (uSpotlight.Kc + uSpotlight.Kl * SpotlightDistance + uSpotlight.Kq * (SpotlightDistance * SpotlightDistance));

	float Theta = dot(SpotlightVector, normalize(-uSpotLightDirection1));
	float Epsilon = uSpotlight.InnerCutOff - uSpotlight.OuterCutOff;
	float SpotIntensity = clamp((Theta - uSpotlight.OuterCutOff) / Epsilon, 0.0f, 1.0f);
	vec3 SpotColor1 =  SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);


	//SpotLight 2
	SpotlightVector = normalize(uSpotLightPosition2 - vWorldSpaceFragment);

	SpotDiffuse = max(dot(vWorldSpaceNormal, SpotlightVector), 0.0f);
	SpotReflectDirection = reflect(-SpotlightVector, vWorldSpaceNormal);
	SpotSpecular = pow(max(dot(ViewDirection, SpotReflectDirection), 0.0f), uMaterial.Shininess);

	SpotAmbientColor = uSpotlight.Ka * vec3(texture(uMaterial.Kd, UV));
	SpotDiffuseColor = SpotDiffuse * uSpotlight.Kd * vec3(texture(uMaterial.Kd, UV));
	SpotSpecularColor = SpotSpecular * uSpotlight.Ks * vec3(texture(uMaterial.Ks, UV));

	SpotlightDistance = length(uSpotLightPosition2 - vWorldSpaceFragment);
	SpotAttenuation = 1.0f / (uSpotlight.Kc + uSpotlight.Kl * SpotlightDistance + uSpotlight.Kq * (SpotlightDistance * SpotlightDistance));

	Theta = dot(SpotlightVector, normalize(-uSpotLightDirection2));
	Epsilon = uSpotlight.InnerCutOff - uSpotlight.OuterCutOff;
	SpotIntensity = clamp((Theta - uSpotlight.OuterCutOff) / Epsilon, 0.0f, 1.0f);
	vec3 SpotColor2 =  SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);
	
	vec3 FinalColor = DirColor + PtColor1 + PtColor2 + PtColor3 +  SpotColor1 +  SpotColor2;
	FragColor = vec4(FinalColor, 1.0f);


}