#version 330

in vec2 texCoord;

out vec4 fragColour;

uniform sampler2D Blur0;
uniform float u_blurWeight;

//----------------------------------------------

vec2 pos[11] = vec2[11]
(
    vec2(-5., 0.),
    vec2(-4., 0.),
    vec2(-3., 0.),
    vec2(-2., 0.),
    vec2(-1., 0.),
    vec2( 0., 0.),
    vec2( 1., 0.),
    vec2( 2., 0.),
    vec2( 3., 0.),
    vec2( 4., 0.),
    vec2( 5., 0.)
);

// We use the Normal-gauss distribution formula
// f(x) being the formula, we used f(0.5)-f(-0.5); f(1.5)-f(0.5)...
float samples[11] = float[11]
(//stddev=2.0
    0.01222447,
    0.02783468,
    0.06559061,
    0.12097757,
    0.17466632,

    0.19741265,

    0.17466632,
    0.12097757,
    0.06559061,
    0.02783468,
    0.01222447
);

//----------------------------------------------

void main()
{
    vec4 sum = vec4(0.);

    for(int i = 0; i < 11; ++i)
    {
        sum += texture(Blur0, texCoord + pos[i] * u_blurWeight) * samples[i];
    }

    fragColour = sum;
}
