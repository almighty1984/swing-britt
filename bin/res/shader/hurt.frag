uniform ivec3 color;
uniform sampler2D texture;
    
    void main(void)
    {
        vec4 finalColor = texture2D(texture, gl_TexCoord[0].st); // This line gets the color from the texture that the BGE renders; as the coordinates, it uses
        // the texture coordinate of each pixel in the viewport - gl_TexCoord[0]. I believe gl_TexCoord[1] is the texture coordinates of each pixel 
        // *in the viewport* once scaled to the size of the viewport 
        
        vec3 newColor = vec3(color.r / 255.0, color.g / 255.0, color.b / 255.0);

        vec3 colorDiff = newColor - vec3(1.0, 1.0, 1.0);
        //finalColor + vec4(gl_FragColor.grb - color, gl_FragColor.a);

        gl_FragColor = vec4(finalColor.grb + colorDiff, finalColor.a);  // This line sets the final color of each pixel (gl_FragColor) to be the color vector, in which r is the red channel,
        // b is the blue, g is the green, and a is the alpha channel. By switching these around, we can manipulate the final color of the screen so that
        // red is now where green should be, blue where green was, and green where red originally is.
    }