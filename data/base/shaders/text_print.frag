// Creative Commons CC0 1.0 Universal (CC-0) 
// https://www.shadertoy.com/view/4sBSWW

// USAGE:         CURRENT    TEXT COLOUR                                     POSITION      FONT SIZE      VALUE   DIGITS DECIMALS         
// vColour = mix( vColour, vec3(1.0, 1.0, 0.0), PrintValue( (fragCoord - vec2(0.0, 5.0)) / vFontSize,    fValue,   4.0,     0.0));

float DigitBin( const int x )
{
    return 
        x==0 ? 480599.0:
    	x==1 ? 139810.0:
    	x==2 ? 476951.0:
    	x==3 ? 476999.0:
    	x==4 ? 350020.0:
    	x==5 ? 464711.0:
    	x==6 ? 464727.0:
    	x==7 ? 476228.0:
    	x==8 ? 481111.0:
    	x==9 ? 481095.0:
        0.0;
}

float PrintValue( vec2 vStringCoords, float fValue, float fMaxDigits, float fDecimalPlaces )
{       
    if ((vStringCoords.y < 0.0) || (vStringCoords.y >= 1.0)) return 0.0;
    
    bool bNeg = ( fValue < 0.0 );
	fValue = abs(fValue);
    
	float fLog10Value = log2(abs(fValue)) / log2(10.0);
	float fBiggestIndex = max(floor(fLog10Value), 0.0);
	float fDigitIndex = fMaxDigits - floor(vStringCoords.x);
	float fCharBin = 0.0;
	if(fDigitIndex > (-fDecimalPlaces - 1.01)) {
		if(fDigitIndex > fBiggestIndex) {
			if((bNeg) && (fDigitIndex < (fBiggestIndex+1.5))) { fCharBin = 1792.0; }
		} else {		
			if(fDigitIndex == -1.0) {
				if(fDecimalPlaces > 0.0) fCharBin = 2.0;
			} else {
                float fReducedRangeValue = fValue;
                if(fDigitIndex < 0.0) { fReducedRangeValue = fract( fValue ); fDigitIndex += 1.0; }
				float fDigitValue = (abs(fReducedRangeValue / (pow(10.0, fDigitIndex))));
                fCharBin = DigitBin(int(floor(mod(fDigitValue, 10.0))));
			}
        }
	}

    return floor(mod((fCharBin / pow(2.0, floor(fract(vStringCoords.x) * 4.0) + (floor(vStringCoords.y * 5.0) * 4.0))), 2.0));
}