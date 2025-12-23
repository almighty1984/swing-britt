varying in vec4 vert_pos;

uniform sampler2D texture;
uniform bool hasTexture;
uniform vec2 lightPos;

void main()
{
	//Ambient light
	vec4 ambient = vec4(0.75, 0.75, 0.75, 0.0);

	lightPos = (gl_ModelViewProjectionMatrix * vec4(lightPos, 0, 1)).xy;
	
	// lookup the pixel in the texture
	//vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

	vec4 output =  vec4(0.0);

	vec2 lightPos2 = lightPos;	
	
	vec2 lightToFrag = lightPos2 - vert_pos.xy;

	for (int i = 0; i < 1; i++) {
		//Convert light to view coords
		
		//Calculate the vector from light to pixel (Make circular)
		
		lightToFrag.x = lightToFrag.x;
		lightToFrag.y = lightToFrag.y / 1.7;


		//Length of the vector (distance)
		

		

		

		float vecLength = clamp(length(lightToFrag) * 2, 0, 1);

		output += ambient + vec4(1-vecLength, 1-vecLength, 1-vecLength, 1);
		
	}

	
	


    // multiply it by the color and lighting
	//if (hasTexture == true)
	//{
		gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * output;
	//}
	//else
	//{
	//	gl_FragColor = gl_Color;
	//}
}