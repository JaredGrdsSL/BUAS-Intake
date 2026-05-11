#version 130

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 circle_pos;
uniform float circle_radius;

void main() {
    float d = distance(gl_FragCoord.xy, circle_pos);

    float factor = clamp(d / circle_radius, 0.0, 1.0);

    float isOuter = step(0.5, factor);
    vec4 gradientColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), factor * 2);

    gl_FragColor = mix(gradientColor, vec4(1), -isOuter);
}