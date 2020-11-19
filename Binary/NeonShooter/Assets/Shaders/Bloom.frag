#version 330
        
in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D image;

void main() {
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    
    vec2 texSize = textureSize(image, 0);
    float size = max(texSize.x, texSize.y);
    
    vec2 texOffset = 1.0 / vec2(size, size);
    vec3 result = texture(image, fragTexCoord).rgb * weight[0];
    
    for (int i = 1; i < 5; ++i) {
        result += texture(image, fragTexCoord + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
        result += texture(image, fragTexCoord - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
        result += texture(image, fragTexCoord + vec2(0.0, texOffset.y * i)).rgb * weight[i];
        result += texture(image, fragTexCoord - vec2(0.0, texOffset.y * i)).rgb * weight[i];
    }

    result = texture(image, fragTexCoord).rgb + result;
    result = vec3(1.0) - exp(-result * 0.5f);
    result = pow(result, vec3(1.0 / 2.2));
    finalColor = vec4(result, 1.0);
};