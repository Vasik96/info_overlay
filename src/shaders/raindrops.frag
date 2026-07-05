#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    vec2 iResolution;
    float iTime;
};

vec3 hash13(float p) {
    vec3 p3 = fract(vec3(p) * vec3(.1031, .11369, .13787));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract(vec3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}

void main() {
    vec2 uv = qt_TexCoord0;
    vec2 res = (iResolution.x > 1.0 && iResolution.y > 1.0) ? iResolution : vec2(1920.0, 1080.0);
    float aspect = res.x / res.y;
    vec2 aspectUv = uv * vec2(aspect, 1.0);

    vec4 finalComposite = vec4(0.0);
    const int MAX_DROPS = 20;

    for (int i = 0; i < MAX_DROPS; i++) {
        vec3 dropIdentity = hash13(float(i) * 73.15 + 42.83);

        float spawnX = dropIdentity.x * aspect;

        // --- FALL SLOWER ---
        // Slashed base speed and variance down by half so they drift down gently
        float speed = 0.04 + (dropIdentity.y * 0.03);
        float size = 0.007 + (dropIdentity.z * 0.014);

        float timeOffset = dropIdentity.y * 11.3;
        float currentProgress = fract((iTime * speed + timeOffset) * 0.1);
        float spawnY = -0.1 + (currentProgress * 1.2);

        float wobble = sin(uv.y * (7.0 + dropIdentity.z * 5.0) + timeOffset) * 0.007;
        vec2 dropCenter = vec2(spawnX + wobble, spawnY);
        vec2 toDrop = aspectUv - dropCenter;

        // Variety & organic shape distortion profiles
        float uniqueFrequency = 25.0 + (dropIdentity.y * 20.0);
        float uniqueAmplitude = 0.15 + (dropIdentity.z * 0.2);

        float shapeDistortion = 1.0 + sin(toDrop.y * uniqueFrequency) * uniqueAmplitude * dropIdentity.x;
        toDrop.x *= shapeDistortion;
        toDrop.y *= (1.0 - toDrop.y * 10.0 * dropIdentity.z);

        // Dynamic single-axis stretch logic
        vec2 stretchFactor = vec2(1.0, 1.0);
        if (dropIdentity.x > 0.4) {
            float dynamicStretch = 1.0 + (dropIdentity.y * 0.5);
            if (dropIdentity.z > 0.5) {
                stretchFactor.y = dynamicStretch;
            } else {
                stretchFactor.x = dynamicStretch;
            }
        }

        toDrop /= stretchFactor;

        float dist = length(toDrop);
        float currentRatio = dist / size;

        if (currentRatio < 1.0) {
            vec2 normal = (toDrop / size) * stretchFactor;
            normal = normalize(normal);

            float edgeFeather = smoothstep(1.0, 0.88, currentRatio);

            // 1. LOWER CONTRAST HIGHLIGHT: Dimmed peak brightness even further
            vec2 lightDir = normalize(vec2(-0.5, -0.5));
            float lightMatch = max(dot(normal, lightDir), 0.0);
            float highlight = pow(lightMatch, 8.0) * 0.22; // Brought down from 0.35 to blend better
            highlight *= smoothstep(0.0, 0.4, lightMatch);

            // 2. MORE VISIBLE BODY (LESS CONTRAST): Raised the base glass profile floor
            // This brings the rest of the droplet's shape into view without blinding highlights
            float subtleBody = (1.0 - currentRatio) * 0.09; // Increased from 0.04 to enhance shape visibility

            // 3. BALANCED RIM GLOW
            float rimGlow = pow(max(dot(normal, vec2(0.5, 0.5)), 0.0), 6.0) * 0.12;
            float microOuterRim = smoothstep(0.92, 0.99, currentRatio) * 0.10;

            vec3 dropRGB = vec3(1.0, 1.0, 1.0);

            // Combine the compressed, low-contrast alpha layout
            float finalAlpha = highlight + subtleBody + rimGlow + microOuterRim;
            finalAlpha *= edgeFeather;

            // Stabilized transparency bounds to lock in the clear glass lens look
            vec4 dropLayer = vec4(dropRGB * finalAlpha, clamp(finalAlpha, 0.09, 0.45));
            finalComposite = dropLayer + finalComposite * (1.0 - dropLayer.a);
        }
    }

    fragColor = finalComposite * qt_Opacity;
}
