#include "badcoiq.h"
BQ_LINK_LIBRARY("badcoiq"); 


#include "badcoiq/system/bqWindow.h"
#include "badcoiq/input/bqInput.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/common/bqImage.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/math/bqVector.h"
#include "badcoiq/containers/bqList.h"
#include "badcoiq/containers/bqArray.h"

#include "badcoiq/GUI/bqGUI.h"

#include "badcoiq/scene/bqCamera.h"
#include <list>

//#include "badcoiq/VG/bqVectorGraphics.h"

// General defines
#ifndef NULL
#define NULL 0
#endif
#define MEMSET(aAddress,aData,aLength) memset(aAddress,aData,aLength)
#define MEMCPY(aTarget,aSource,aLength) memcpy(aTarget,aSource,aLength)
#define STRDUP(aString) strdup(aString)
#define STRCAT(aTgt,aSrc) strcat(aTgt,aSrc)
#define STRLEN(aString) strlen(aString)
#define STREQ(aString1,aString2) (strcmp(aString1,aString2) == 0)
#define SSCANF(aString,aFormat,aParameter) sscanf(aString,aFormat,aParameter)

// Defines for the rational number format.
#define RATIONAL float
#define INT_TO_RATIONAL(a) ((float)(a))
#define FLOAT_TO_RATIONAL(a) (a)
#define RATIONAL_SIN(a) (float)sin((double)(a))
#define RATIONAL_COS(a) (float)cos((double)(a))
#define MIN_RATIONAL ((float)-1000000000000000)
#define MAX_RATIONAL ((float)1000000000000000)

/*
#define RATIONAL double
#define INT_TO_RATIONAL(a) ((double)(a))
#define FLOAT_TO_RATIONAL(a) ((double)(a))
#define RATIONAL_SIN(a) sin((a))
#define RATIONAL_COS(a) cos((a))
#define MIN_RATIONAL ((double)-1000000000000000)
#define MAX_RATIONAL ((double)1000000000000000)
*/

// Defines the fixed point conversions
#define FIXED_POINT int
#define FIXED_POINT_SHIFT 16
#define INT_TO_FIXED(a) ((a) << FIXED_POINT_SHIFT)
#define FLOAT_TO_FIXED(a) (int)((a)*((float)(1 << FIXED_POINT_SHIFT)))

#define FIXED_TO_INT(a) ((a) >> FIXED_POINT_SHIFT) 
#define FIXED_TO_FLOAT(a) ((float)(a)/((float)(1 << FIXED_POINT_SHIFT)))
#define RATIONAL_TO_FIXED(a) FLOAT_TO_FIXED(a)
#define FIXED_TO_RATIONAL(a) FIXED_TO_FLOAT(a)

// Define for 64-bit integer number format
#define INTEGER64 __int64

class Vector2d
{
public:
    //! Constructor.
    inline Vector2d() {}

    //! Constructor.
    inline Vector2d(RATIONAL aX, RATIONAL aY)
    {
        mX = aX;
        mY = aY;
    }

    RATIONAL mX = 0.f;
    RATIONAL mY = 0.f;
};
class Matrix2d
{
public:
    Matrix2d()
    {
        makeIdentity();
    }

    // Makes a rotation matrix.
    void makeRotation(RATIONAL aAngle) {
        RATIONAL sinRot = (RATIONAL)sin(aAngle);
        RATIONAL cosRot = (RATIONAL)cos(aAngle);

        mMatrix[0][0] = cosRot;
        mMatrix[0][1] = sinRot;
        mMatrix[1][0] = -sinRot;
        mMatrix[1][1] = cosRot;
        mMatrix[2][0] = INT_TO_RATIONAL(0);
        mMatrix[2][1] = INT_TO_RATIONAL(0);
    }

    // Makes a translation matrix.
    void makeTranslation(const Vector2d& aTranslation) {
        mMatrix[0][0] = INT_TO_RATIONAL(1);
        mMatrix[0][1] = INT_TO_RATIONAL(0);
        mMatrix[1][0] = INT_TO_RATIONAL(0);
        mMatrix[1][1] = INT_TO_RATIONAL(1);
        mMatrix[2][0] = aTranslation.mX;
        mMatrix[2][1] = aTranslation.mY;
    }

    // Makes a scaling matrix.
    void makeScaling(const Vector2d& aScale) {
        mMatrix[0][0] = aScale.mX;
        mMatrix[0][1] = INT_TO_RATIONAL(0);
        mMatrix[1][0] = INT_TO_RATIONAL(0);
        mMatrix[1][1] = aScale.mY;
        mMatrix[2][0] = INT_TO_RATIONAL(0);
        mMatrix[2][1] = INT_TO_RATIONAL(0);
    }

    // Makes an identity matrix.
    void makeIdentity() {
        mMatrix[0][0] = INT_TO_RATIONAL(1);
        mMatrix[0][1] = INT_TO_RATIONAL(0);
        mMatrix[1][0] = INT_TO_RATIONAL(0);
        mMatrix[1][1] = INT_TO_RATIONAL(1);
        mMatrix[2][0] = INT_TO_RATIONAL(0);
        mMatrix[2][1] = INT_TO_RATIONAL(0);
    }

    // Rotates the matrix by an angle
    void rotate(RATIONAL rotate) {
        Matrix2d tmp;
        tmp.makeRotation(rotate);
        multiply(tmp);
    }

    // Translates the matrix with a vector.
    void translate(const Vector2d& aTranslation) {
        Matrix2d tmp;
        tmp.makeTranslation(aTranslation);
        multiply(tmp);
    }

    // Scales the matrix with a vector.
    void scale(const Vector2d& aScale) {
        Matrix2d tmp;
        tmp.makeScaling(aScale);
        multiply(tmp);
    }

    // Multiplies the matrix with another matrix.
    void multiply(const Matrix2d& aMatrix) {
        Matrix2d tmp = *this;
        mMatrix[0][0] = tmp.mMatrix[0][0] * aMatrix.mMatrix[0][0] +
            tmp.mMatrix[0][1] * aMatrix.mMatrix[1][0];
        mMatrix[0][1] = tmp.mMatrix[0][0] * aMatrix.mMatrix[0][1] +
            tmp.mMatrix[0][1] * aMatrix.mMatrix[1][1];
        mMatrix[1][0] = tmp.mMatrix[1][0] * aMatrix.mMatrix[0][0] +
            tmp.mMatrix[1][1] * aMatrix.mMatrix[1][0];
        mMatrix[1][1] = tmp.mMatrix[1][0] * aMatrix.mMatrix[0][1] +
            tmp.mMatrix[1][1] * aMatrix.mMatrix[1][1];
        mMatrix[2][0] = tmp.mMatrix[2][0] * aMatrix.mMatrix[0][0] +
            tmp.mMatrix[2][1] * aMatrix.mMatrix[1][0] +
            aMatrix.mMatrix[2][0];
        mMatrix[2][1] = tmp.mMatrix[2][0] * aMatrix.mMatrix[0][1] +
            tmp.mMatrix[2][1] * aMatrix.mMatrix[1][1] +
            aMatrix.mMatrix[2][1];
    }

    // Transforms a point with the matrix.
    void transform(const Vector2d& aSource, Vector2d& aTarget) const {
        aTarget.mX = mMatrix[0][0] * aSource.mX +
            mMatrix[1][0] * aSource.mY +
            mMatrix[2][0];

        aTarget.mY = mMatrix[0][1] * aSource.mX +
            mMatrix[1][1] * aSource.mY +
            mMatrix[2][1];
    }

    RATIONAL mMatrix[3][2];
};

#define VECTOR2D Vector2d
#define VECTOR2D_SETX(aVector,aX) aVector.mX = (aX)
#define VECTOR2D_SETY(aVector,aY) aVector.mY = (aY)
#define VECTOR2D_GETX(aVector) aVector.mX
#define VECTOR2D_GETY(aVector) aVector.mY

#define MATRIX2D Matrix2d
#define MATRIX2D_MAKEROTATION(aMatrix,aAngle) aMatrix.makeRotation(aAngle)
#define MATRIX2D_MAKETRANSLATION(aMatrix,aTranslation) aMatrix.makeTranslation(aTranslation)
#define MATRIX2D_MAKESCALING(aMatrix,aScale) aMatrix.makeScaling(aScale)
#define MATRIX2D_MAKEIDENTITY(aMatrix) aMatrix.makeIdentity()
#define MATRIX2D_ROTATE(aMatrix,aAngle) aMatrix.rotate(aAngle)
#define MATRIX2D_TRANSLATE(aMatrix,aTranslation) aMatrix.translate(aTranslation)
#define MATRIX2D_SCALE(aMatrix,aScale) aMatrix.scale(aScale)
#define MATRIX2D_MULTIPLY(aMatrix,aMultiplier) aMatrix.multiply(aMultiplier)
#define MATRIX2D_TRANSFORM(aMatrix,aSource,aResult) aMatrix.transform(aSource,aResult)
#define MATRIX2D_GET_M11(aMatrix) aMatrix.mMatrix[0][0]
#define MATRIX2D_GET_M12(aMatrix) aMatrix.mMatrix[0][1]
#define MATRIX2D_GET_M21(aMatrix) aMatrix.mMatrix[1][0]
#define MATRIX2D_GET_M22(aMatrix) aMatrix.mMatrix[1][1]
#define MATRIX2D_GET_DX(aMatrix) aMatrix.mMatrix[2][0]
#define MATRIX2D_GET_DY(aMatrix) aMatrix.mMatrix[2][1]

// Defines the file IO functions for writing text
#define TEXTFILE_OBJECT FILE
#define TEXTFILE_OPEN_FOR_WRITE(aName) fopen(aName,"wb")
#define TEXTFILE_WRITE_STRING(aFile,aString) fprintf(aFile,"%s",aString)
#define TEXTFILE_WRITE_RATIONAL(aFile,aRational) fprintf(aFile,"%f",(float)aRational)
#define TEXTFILE_WRITE_INT(aFile,aInt) fprintf(aFile,"%d",aInt)
#define TEXTFILE_CLOSE(aObject) fclose(aObject)

// Defines the file IO functions for writing binary
#define BINARYFILE_OBJECT FILE
#define BINARYFILE_OPEN_FOR_WRITE(aName) fopen(aName,"wb")
#define BINARYFILE_WRITE_DATA(aDataPtr,aItemSize,aItemCount,aFile) fwrite(aDataPtr,aItemSize,aItemCount,aFile)
#define BINARYFILE_CLOSE(aObject) fclose(aObject)

// Defines the debug assertion
#define DEBUG_ASSERT(aExp) assert(aExp)



// Main define for data format
// Default value is char for data.
// #define NON_ZERO_MASK_USE_SHORT_DATA

// More defines built from these
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
#define NON_ZERO_MASK_DATA_UNIT char
#define NON_ZERO_MASK_DATA_BITS 8
#else
#define NON_ZERO_MASK_DATA_UNIT short
#define NON_ZERO_MASK_DATA_BITS 16
#endif

/*! Fixed point math inevitably introduces rounding error to the DDA. The error is
 *  fixed every now and then by a separate fix value. The defines below set these.
 */
#define SLOPE_FIX_SHIFT 8
#define SLOPE_FIX_STEP (1 << SLOPE_FIX_SHIFT)
#define SLOPE_FIX_MASK (SLOPE_FIX_STEP - 1)
#define SLOPE_FIX_SCANLINES (1 << (SLOPE_FIX_SHIFT - SUBPIXEL_SHIFT))
#define SLOPE_FIX_SCANLINE_MASK (SLOPE_FIX_SCANLINES - 1)
#define SUBPIXEL_SHIFT 3
 /*
 8x8 sparse supersampling mask:

 [][][][][]##[][] 5
 ##[][][][][][][] 0
 [][][]##[][][][] 3
 [][][][][][]##[] 6
 []##[][][][][][] 1
 [][][][]##[][][] 4
 [][][][][][][]## 7
 [][]##[][][][][] 2

 16x16 sparse supersampling mask:

 []##[][][][][][][][][][][][][][] 1
 [][][][][][][][]##[][][][][][][] 8
 [][][][]##[][][][][][][][][][][] 4
 [][][][][][][][][][][][][][][]## 15
 [][][][][][][][][][][]##[][][][] 11
 [][]##[][][][][][][][][][][][][] 2
 [][][][][][]##[][][][][][][][][] 6
 [][][][][][][][][][][][][][]##[] 14
 [][][][][][][][][][]##[][][][][] 10
 [][][]##[][][][][][][][][][][][] 3
 [][][][][][][]##[][][][][][][][] 7
 [][][][][][][][][][][][]##[][][] 12
 ##[][][][][][][][][][][][][][][] 0
 [][][][][][][][][]##[][][][][][] 9
 [][][][][]##[][][][][][][][][][] 5
 [][][][][][][][][][][][][]##[][] 13

 32x32 sparse supersampling mask

 [][][][][][][][][][][][][][][][][][][][][][][][][][][][]##[][][] 28
 [][][][][][][][][][][][][]##[][][][][][][][][][][][][][][][][][] 13
 [][][][][][]##[][][][][][][][][][][][][][][][][][][][][][][][][] 6
 [][][][][][][][][][][][][][][][][][][][][][][]##[][][][][][][][] 23
 ##[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][] 0
 [][][][][][][][][][][][][][][][][]##[][][][][][][][][][][][][][] 17
 [][][][][][][][][][]##[][][][][][][][][][][][][][][][][][][][][] 10
 [][][][][][][][][][][][][][][][][][][][][][][][][][][]##[][][][] 27
 [][][][]##[][][][][][][][][][][][][][][][][][][][][][][][][][][] 4
 [][][][][][][][][][][][][][][][][][][][][]##[][][][][][][][][][] 21
 [][][][][][][][][][][][][][]##[][][][][][][][][][][][][][][][][] 14
 [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]## 31
 [][][][][][][][]##[][][][][][][][][][][][][][][][][][][][][][][] 8
 [][][][][][][][][][][][][][][][][][][][][][][][][]##[][][][][][] 25
 [][][][][][][][][][][][][][][][][][]##[][][][][][][][][][][][][] 18
 [][][]##[][][][][][][][][][][][][][][][][][][][][][][][][][][][] 3
 [][][][][][][][][][][][]##[][][][][][][][][][][][][][][][][][][] 12
 [][][][][][][][][][][][][][][][][][][][][][][][][][][][][]##[][] 29
 [][][][][][][][][][][][][][][][][][][][][][]##[][][][][][][][][] 22
 [][][][][][][]##[][][][][][][][][][][][][][][][][][][][][][][][] 7
 [][][][][][][][][][][][][][][][]##[][][][][][][][][][][][][][][] 16
 []##[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][] 1
 [][][][][][][][][][][][][][][][][][][][][][][][][][]##[][][][][] 26
 [][][][][][][][][][][]##[][][][][][][][][][][][][][][][][][][][] 11
 [][][][][][][][][][][][][][][][][][][][]##[][][][][][][][][][][] 20
 [][][][][]##[][][][][][][][][][][][][][][][][][][][][][][][][][] 5
 [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]##[] 30
 [][][][][][][][][][][][][][][]##[][][][][][][][][][][][][][][][] 15
 [][][][][][][][][][][][][][][][][][][][][][][][]##[][][][][][][] 24
 [][][][][][][][][]##[][][][][][][][][][][][][][][][][][][][][][] 9
 [][]##[][][][][][][][][][][][][][][][][][][][][][][][][][][][][] 2
 [][][][][][][][][][][][][][][][][][][]##[][][][][][][][][][][][] 19
*/

#define SUBPIXEL_COUNT (1 << SUBPIXEL_SHIFT)

#if SUBPIXEL_SHIFT == 3
#define SUBPIXEL_DATA unsigned char
#define WINDING_DATA unsigned __int64
#define SUBPIXEL_COVERAGE(a) (coverageTable[(a)])

#define SUBPIXEL_OFFSET_0 (5.0f/8.0f)
#define SUBPIXEL_OFFSET_1 (0.0f/8.0f)
#define SUBPIXEL_OFFSET_2 (3.0f/8.0f)
#define SUBPIXEL_OFFSET_3 (6.0f/8.0f)
#define SUBPIXEL_OFFSET_4 (1.0f/8.0f)
#define SUBPIXEL_OFFSET_5 (4.0f/8.0f)
#define SUBPIXEL_OFFSET_6 (7.0f/8.0f)
#define SUBPIXEL_OFFSET_7 (2.0f/8.0f)

#define SUBPIXEL_OFFSET_RATIONAL_0 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_RATIONAL_1 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_RATIONAL_2 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_RATIONAL_3 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_RATIONAL_4 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_RATIONAL_5 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_RATIONAL_6 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_RATIONAL_7 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_7)

#define SUBPIXEL_OFFSET_FIXED_0 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_FIXED_1 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_FIXED_2 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_FIXED_3 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_FIXED_4 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_FIXED_5 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_FIXED_6 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_FIXED_7 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_7)

#define SUBPIXEL_OFFSETS { SUBPIXEL_OFFSET_RATIONAL_0,\
                           SUBPIXEL_OFFSET_RATIONAL_1,\
                           SUBPIXEL_OFFSET_RATIONAL_2,\
                           SUBPIXEL_OFFSET_RATIONAL_3,\
                           SUBPIXEL_OFFSET_RATIONAL_4,\
                           SUBPIXEL_OFFSET_RATIONAL_5,\
                           SUBPIXEL_OFFSET_RATIONAL_6,\
                           SUBPIXEL_OFFSET_RATIONAL_7 }

#define SUBPIXEL_OFFSETS_FIXED { SUBPIXEL_OFFSET_FIXED_0,\
                                 SUBPIXEL_OFFSET_FIXED_1,\
                                 SUBPIXEL_OFFSET_FIXED_2,\
                                 SUBPIXEL_OFFSET_FIXED_3,\
                                 SUBPIXEL_OFFSET_FIXED_4,\
                                 SUBPIXEL_OFFSET_FIXED_5,\
                                 SUBPIXEL_OFFSET_FIXED_6,\
                                 SUBPIXEL_OFFSET_FIXED_7 }

#define SUBPIXEL_FULL_COVERAGE 0xff
#elif SUBPIXEL_SHIFT == 4
#define SUBPIXEL_DATA unsigned short
#define SUBPIXEL_COVERAGE(a) (coverageTable[(a) & 0xff] + coverageTable[((a) >> 8) & 0xff])

#define SUBPIXEL_OFFSET_0  (1.0f/16.0f)
#define SUBPIXEL_OFFSET_1  (8.0f/16.0f)
#define SUBPIXEL_OFFSET_2  (4.0f/16.0f)
#define SUBPIXEL_OFFSET_3  (15.0f/16.0f)
#define SUBPIXEL_OFFSET_4  (11.0f/16.0f)
#define SUBPIXEL_OFFSET_5  (2.0f/16.0f)
#define SUBPIXEL_OFFSET_6  (6.0f/16.0f)
#define SUBPIXEL_OFFSET_7  (14.0f/16.0f)
#define SUBPIXEL_OFFSET_8  (10.0f/16.0f)
#define SUBPIXEL_OFFSET_9  (3.0f/16.0f)
#define SUBPIXEL_OFFSET_10 (7.0f/16.0f)
#define SUBPIXEL_OFFSET_11 (12.0f/16.0f)
#define SUBPIXEL_OFFSET_12 (0.0f/16.0f)
#define SUBPIXEL_OFFSET_13 (9.0f/16.0f)
#define SUBPIXEL_OFFSET_14 (5.0f/16.0f)
#define SUBPIXEL_OFFSET_15 (13.0f/16.0f)

#define SUBPIXEL_OFFSET_RATIONAL_0 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_RATIONAL_1 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_RATIONAL_2 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_RATIONAL_3 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_RATIONAL_4 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_RATIONAL_5 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_RATIONAL_6 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_RATIONAL_7 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_RATIONAL_8 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_RATIONAL_9 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_RATIONAL_10 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_RATIONAL_11 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_RATIONAL_12 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_RATIONAL_13 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_RATIONAL_14 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_RATIONAL_15 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_15)

#define SUBPIXEL_OFFSET_FIXED_0 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_FIXED_1 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_FIXED_2 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_FIXED_3 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_FIXED_4 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_FIXED_5 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_FIXED_6 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_FIXED_7 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_FIXED_8 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_FIXED_9 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_FIXED_10 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_FIXED_11 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_FIXED_12 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_FIXED_13 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_FIXED_14 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_FIXED_15 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_15)

#define SUBPIXEL_OFFSETS { SUBPIXEL_OFFSET_RATIONAL_0,\
                           SUBPIXEL_OFFSET_RATIONAL_1,\
                           SUBPIXEL_OFFSET_RATIONAL_2,\
                           SUBPIXEL_OFFSET_RATIONAL_3,\
                           SUBPIXEL_OFFSET_RATIONAL_4,\
                           SUBPIXEL_OFFSET_RATIONAL_5,\
                           SUBPIXEL_OFFSET_RATIONAL_6,\
                           SUBPIXEL_OFFSET_RATIONAL_7,\
                           SUBPIXEL_OFFSET_RATIONAL_8,\
                           SUBPIXEL_OFFSET_RATIONAL_9,\
                           SUBPIXEL_OFFSET_RATIONAL_10,\
                           SUBPIXEL_OFFSET_RATIONAL_11,\
                           SUBPIXEL_OFFSET_RATIONAL_12,\
                           SUBPIXEL_OFFSET_RATIONAL_13,\
                           SUBPIXEL_OFFSET_RATIONAL_14,\
                           SUBPIXEL_OFFSET_RATIONAL_15 }

#define SUBPIXEL_OFFSETS_FIXED { SUBPIXEL_OFFSET_FIXED_0,\
                                 SUBPIXEL_OFFSET_FIXED_1,\
                                 SUBPIXEL_OFFSET_FIXED_2,\
                                 SUBPIXEL_OFFSET_FIXED_3,\
                                 SUBPIXEL_OFFSET_FIXED_4,\
                                 SUBPIXEL_OFFSET_FIXED_5,\
                                 SUBPIXEL_OFFSET_FIXED_6,\
                                 SUBPIXEL_OFFSET_FIXED_7,\
                                 SUBPIXEL_OFFSET_FIXED_8,\
                                 SUBPIXEL_OFFSET_FIXED_9,\
                                 SUBPIXEL_OFFSET_FIXED_10,\
                                 SUBPIXEL_OFFSET_FIXED_11,\
                                 SUBPIXEL_OFFSET_FIXED_12,\
                                 SUBPIXEL_OFFSET_FIXED_13,\
                                 SUBPIXEL_OFFSET_FIXED_14,\
                                 SUBPIXEL_OFFSET_FIXED_15 }

#define SUBPIXEL_FULL_COVERAGE 0xffff
#elif SUBPIXEL_SHIFT == 5
#define SUBPIXEL_DATA unsigned long
#define SUBPIXEL_COVERAGE(a) (coverageTable[(a) & 0xff] + coverageTable[((a) >> 8) & 0xff] + coverageTable[((a) >> 16) & 0xff] + coverageTable[((a) >> 24) & 0xff])

#define SUBPIXEL_OFFSET_0  (28.0f/32.0f)
#define SUBPIXEL_OFFSET_1  (13.0f/32.0f)
#define SUBPIXEL_OFFSET_2  (6.0f/32.0f)
#define SUBPIXEL_OFFSET_3  (23.0f/32.0f)
#define SUBPIXEL_OFFSET_4  (0.0f/32.0f)
#define SUBPIXEL_OFFSET_5  (17.0f/32.0f)
#define SUBPIXEL_OFFSET_6  (10.0f/32.0f)
#define SUBPIXEL_OFFSET_7  (27.0f/32.0f)
#define SUBPIXEL_OFFSET_8  (4.0f/32.0f)
#define SUBPIXEL_OFFSET_9  (21.0f/32.0f)
#define SUBPIXEL_OFFSET_10 (14.0f/32.0f)
#define SUBPIXEL_OFFSET_11 (31.0f/32.0f)
#define SUBPIXEL_OFFSET_12 (8.0f/32.0f)
#define SUBPIXEL_OFFSET_13 (25.0f/32.0f)
#define SUBPIXEL_OFFSET_14 (18.0f/32.0f)
#define SUBPIXEL_OFFSET_15 (3.0f/32.0f)
#define SUBPIXEL_OFFSET_16 (12.0f/32.0f)
#define SUBPIXEL_OFFSET_17 (29.0f/32.0f)
#define SUBPIXEL_OFFSET_18 (22.0f/32.0f)
#define SUBPIXEL_OFFSET_19 (7.0f/32.0f)
#define SUBPIXEL_OFFSET_20 (16.0f/32.0f)
#define SUBPIXEL_OFFSET_21 (1.0f/32.0f)
#define SUBPIXEL_OFFSET_22 (26.0f/32.0f)
#define SUBPIXEL_OFFSET_23 (11.0f/32.0f)
#define SUBPIXEL_OFFSET_24 (20.0f/32.0f)
#define SUBPIXEL_OFFSET_25 (5.0f/32.0f)
#define SUBPIXEL_OFFSET_26 (30.0f/32.0f)
#define SUBPIXEL_OFFSET_27 (15.0f/32.0f)
#define SUBPIXEL_OFFSET_28 (24.0f/32.0f)
#define SUBPIXEL_OFFSET_29 (9.0f/32.0f)
#define SUBPIXEL_OFFSET_30 (2.0f/32.0f)
#define SUBPIXEL_OFFSET_31 (19.0f/32.0f)

#define SUBPIXEL_OFFSET_RATIONAL_0 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_RATIONAL_1 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_RATIONAL_2 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_RATIONAL_3 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_RATIONAL_4 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_RATIONAL_5 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_RATIONAL_6 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_RATIONAL_7 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_RATIONAL_8 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_RATIONAL_9 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_RATIONAL_10 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_RATIONAL_11 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_RATIONAL_12 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_RATIONAL_13 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_RATIONAL_14 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_RATIONAL_15 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_15)
#define SUBPIXEL_OFFSET_RATIONAL_16 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_16)
#define SUBPIXEL_OFFSET_RATIONAL_17 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_17)
#define SUBPIXEL_OFFSET_RATIONAL_18 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_18)
#define SUBPIXEL_OFFSET_RATIONAL_19 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_19)
#define SUBPIXEL_OFFSET_RATIONAL_20 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_20)
#define SUBPIXEL_OFFSET_RATIONAL_21 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_21)
#define SUBPIXEL_OFFSET_RATIONAL_22 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_22)
#define SUBPIXEL_OFFSET_RATIONAL_23 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_23)
#define SUBPIXEL_OFFSET_RATIONAL_24 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_24)
#define SUBPIXEL_OFFSET_RATIONAL_25 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_25)
#define SUBPIXEL_OFFSET_RATIONAL_26 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_26)
#define SUBPIXEL_OFFSET_RATIONAL_27 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_27)
#define SUBPIXEL_OFFSET_RATIONAL_28 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_28)
#define SUBPIXEL_OFFSET_RATIONAL_29 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_29)
#define SUBPIXEL_OFFSET_RATIONAL_30 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_30)
#define SUBPIXEL_OFFSET_RATIONAL_31 FLOAT_TO_RATIONAL(SUBPIXEL_OFFSET_31)

#define SUBPIXEL_OFFSET_FIXED_0 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_FIXED_1 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_FIXED_2 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_FIXED_3 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_FIXED_4 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_FIXED_5 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_FIXED_6 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_FIXED_7 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_7)
#define SUBPIXEL_OFFSET_FIXED_8 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_8)
#define SUBPIXEL_OFFSET_FIXED_9 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_9)
#define SUBPIXEL_OFFSET_FIXED_10 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_10)
#define SUBPIXEL_OFFSET_FIXED_11 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_11)
#define SUBPIXEL_OFFSET_FIXED_12 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_12)
#define SUBPIXEL_OFFSET_FIXED_13 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_13)
#define SUBPIXEL_OFFSET_FIXED_14 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_14)
#define SUBPIXEL_OFFSET_FIXED_15 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_15)
#define SUBPIXEL_OFFSET_FIXED_16 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_16)
#define SUBPIXEL_OFFSET_FIXED_17 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_17)
#define SUBPIXEL_OFFSET_FIXED_18 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_18)
#define SUBPIXEL_OFFSET_FIXED_19 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_19)
#define SUBPIXEL_OFFSET_FIXED_20 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_20)
#define SUBPIXEL_OFFSET_FIXED_21 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_21)
#define SUBPIXEL_OFFSET_FIXED_22 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_22)
#define SUBPIXEL_OFFSET_FIXED_23 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_23)
#define SUBPIXEL_OFFSET_FIXED_24 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_24)
#define SUBPIXEL_OFFSET_FIXED_25 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_25)
#define SUBPIXEL_OFFSET_FIXED_26 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_26)
#define SUBPIXEL_OFFSET_FIXED_27 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_27)
#define SUBPIXEL_OFFSET_FIXED_28 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_28)
#define SUBPIXEL_OFFSET_FIXED_29 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_29)
#define SUBPIXEL_OFFSET_FIXED_30 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_30)
#define SUBPIXEL_OFFSET_FIXED_31 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_31)

#define SUBPIXEL_OFFSETS { SUBPIXEL_OFFSET_RATIONAL_0,\
                           SUBPIXEL_OFFSET_RATIONAL_1,\
                           SUBPIXEL_OFFSET_RATIONAL_2,\
                           SUBPIXEL_OFFSET_RATIONAL_3,\
                           SUBPIXEL_OFFSET_RATIONAL_4,\
                           SUBPIXEL_OFFSET_RATIONAL_5,\
                           SUBPIXEL_OFFSET_RATIONAL_6,\
                           SUBPIXEL_OFFSET_RATIONAL_7,\
                           SUBPIXEL_OFFSET_RATIONAL_8,\
                           SUBPIXEL_OFFSET_RATIONAL_9,\
                           SUBPIXEL_OFFSET_RATIONAL_10,\
                           SUBPIXEL_OFFSET_RATIONAL_11,\
                           SUBPIXEL_OFFSET_RATIONAL_12,\
                           SUBPIXEL_OFFSET_RATIONAL_13,\
                           SUBPIXEL_OFFSET_RATIONAL_14,\
                           SUBPIXEL_OFFSET_RATIONAL_15,\
                           SUBPIXEL_OFFSET_RATIONAL_16,\
                           SUBPIXEL_OFFSET_RATIONAL_17,\
                           SUBPIXEL_OFFSET_RATIONAL_18,\
                           SUBPIXEL_OFFSET_RATIONAL_19,\
                           SUBPIXEL_OFFSET_RATIONAL_20,\
                           SUBPIXEL_OFFSET_RATIONAL_21,\
                           SUBPIXEL_OFFSET_RATIONAL_22,\
                           SUBPIXEL_OFFSET_RATIONAL_23,\
                           SUBPIXEL_OFFSET_RATIONAL_24,\
                           SUBPIXEL_OFFSET_RATIONAL_25,\
                           SUBPIXEL_OFFSET_RATIONAL_26,\
                           SUBPIXEL_OFFSET_RATIONAL_27,\
                           SUBPIXEL_OFFSET_RATIONAL_28,\
                           SUBPIXEL_OFFSET_RATIONAL_29,\
                           SUBPIXEL_OFFSET_RATIONAL_30,\
                           SUBPIXEL_OFFSET_RATIONAL_31 }

#define SUBPIXEL_OFFSETS_FIXED { SUBPIXEL_OFFSET_FIXED_0,\
                                 SUBPIXEL_OFFSET_FIXED_1,\
                                 SUBPIXEL_OFFSET_FIXED_2,\
                                 SUBPIXEL_OFFSET_FIXED_3,\
                                 SUBPIXEL_OFFSET_FIXED_4,\
                                 SUBPIXEL_OFFSET_FIXED_5,\
                                 SUBPIXEL_OFFSET_FIXED_6,\
                                 SUBPIXEL_OFFSET_FIXED_7,\
                                 SUBPIXEL_OFFSET_FIXED_8,\
                                 SUBPIXEL_OFFSET_FIXED_9,\
                                 SUBPIXEL_OFFSET_FIXED_10,\
                                 SUBPIXEL_OFFSET_FIXED_11,\
                                 SUBPIXEL_OFFSET_FIXED_12,\
                                 SUBPIXEL_OFFSET_FIXED_13,\
                                 SUBPIXEL_OFFSET_FIXED_14,\
                                 SUBPIXEL_OFFSET_FIXED_15,\
                                 SUBPIXEL_OFFSET_FIXED_16,\
                                 SUBPIXEL_OFFSET_FIXED_17,\
                                 SUBPIXEL_OFFSET_FIXED_18,\
                                 SUBPIXEL_OFFSET_FIXED_19,\
                                 SUBPIXEL_OFFSET_FIXED_20,\
                                 SUBPIXEL_OFFSET_FIXED_21,\
                                 SUBPIXEL_OFFSET_FIXED_22,\
                                 SUBPIXEL_OFFSET_FIXED_23,\
                                 SUBPIXEL_OFFSET_FIXED_24,\
                                 SUBPIXEL_OFFSET_FIXED_25,\
                                 SUBPIXEL_OFFSET_FIXED_26,\
                                 SUBPIXEL_OFFSET_FIXED_27,\
                                 SUBPIXEL_OFFSET_FIXED_28,\
                                 SUBPIXEL_OFFSET_FIXED_29,\
                                 SUBPIXEL_OFFSET_FIXED_30,\
                                 SUBPIXEL_OFFSET_FIXED_31 }

#define SUBPIXEL_FULL_COVERAGE 0xffffffff
#endif

#define PIXEL_COVERAGE(a) (((a) & 1) + (((a) >> 1) & 1) + (((a) >> 2) & 1) + (((a) >> 3) & 1) +\
                           (((a) >> 4) & 1) + (((a) >> 5) & 1) + (((a) >> 6) & 1) + (((a) >> 7) & 1))

static const unsigned char coverageTable[256] = {
    PIXEL_COVERAGE(0x00),PIXEL_COVERAGE(0x01),PIXEL_COVERAGE(0x02),PIXEL_COVERAGE(0x03),
    PIXEL_COVERAGE(0x04),PIXEL_COVERAGE(0x05),PIXEL_COVERAGE(0x06),PIXEL_COVERAGE(0x07),
    PIXEL_COVERAGE(0x08),PIXEL_COVERAGE(0x09),PIXEL_COVERAGE(0x0a),PIXEL_COVERAGE(0x0b),
    PIXEL_COVERAGE(0x0c),PIXEL_COVERAGE(0x0d),PIXEL_COVERAGE(0x0e),PIXEL_COVERAGE(0x0f),
    PIXEL_COVERAGE(0x10),PIXEL_COVERAGE(0x11),PIXEL_COVERAGE(0x12),PIXEL_COVERAGE(0x13),
    PIXEL_COVERAGE(0x14),PIXEL_COVERAGE(0x15),PIXEL_COVERAGE(0x16),PIXEL_COVERAGE(0x17),
    PIXEL_COVERAGE(0x18),PIXEL_COVERAGE(0x19),PIXEL_COVERAGE(0x1a),PIXEL_COVERAGE(0x1b),
    PIXEL_COVERAGE(0x1c),PIXEL_COVERAGE(0x1d),PIXEL_COVERAGE(0x1e),PIXEL_COVERAGE(0x1f),
    PIXEL_COVERAGE(0x20),PIXEL_COVERAGE(0x21),PIXEL_COVERAGE(0x22),PIXEL_COVERAGE(0x23),
    PIXEL_COVERAGE(0x24),PIXEL_COVERAGE(0x25),PIXEL_COVERAGE(0x26),PIXEL_COVERAGE(0x27),
    PIXEL_COVERAGE(0x28),PIXEL_COVERAGE(0x29),PIXEL_COVERAGE(0x2a),PIXEL_COVERAGE(0x2b),
    PIXEL_COVERAGE(0x2c),PIXEL_COVERAGE(0x2d),PIXEL_COVERAGE(0x2e),PIXEL_COVERAGE(0x2f),
    PIXEL_COVERAGE(0x30),PIXEL_COVERAGE(0x31),PIXEL_COVERAGE(0x32),PIXEL_COVERAGE(0x33),
    PIXEL_COVERAGE(0x34),PIXEL_COVERAGE(0x35),PIXEL_COVERAGE(0x36),PIXEL_COVERAGE(0x37),
    PIXEL_COVERAGE(0x38),PIXEL_COVERAGE(0x39),PIXEL_COVERAGE(0x3a),PIXEL_COVERAGE(0x3b),
    PIXEL_COVERAGE(0x3c),PIXEL_COVERAGE(0x3d),PIXEL_COVERAGE(0x3e),PIXEL_COVERAGE(0x3f),
    PIXEL_COVERAGE(0x40),PIXEL_COVERAGE(0x41),PIXEL_COVERAGE(0x42),PIXEL_COVERAGE(0x43),
    PIXEL_COVERAGE(0x44),PIXEL_COVERAGE(0x45),PIXEL_COVERAGE(0x46),PIXEL_COVERAGE(0x47),
    PIXEL_COVERAGE(0x48),PIXEL_COVERAGE(0x49),PIXEL_COVERAGE(0x4a),PIXEL_COVERAGE(0x4b),
    PIXEL_COVERAGE(0x4c),PIXEL_COVERAGE(0x4d),PIXEL_COVERAGE(0x4e),PIXEL_COVERAGE(0x4f),
    PIXEL_COVERAGE(0x50),PIXEL_COVERAGE(0x51),PIXEL_COVERAGE(0x52),PIXEL_COVERAGE(0x53),
    PIXEL_COVERAGE(0x54),PIXEL_COVERAGE(0x55),PIXEL_COVERAGE(0x56),PIXEL_COVERAGE(0x57),
    PIXEL_COVERAGE(0x58),PIXEL_COVERAGE(0x59),PIXEL_COVERAGE(0x5a),PIXEL_COVERAGE(0x5b),
    PIXEL_COVERAGE(0x5c),PIXEL_COVERAGE(0x5d),PIXEL_COVERAGE(0x5e),PIXEL_COVERAGE(0x5f),
    PIXEL_COVERAGE(0x60),PIXEL_COVERAGE(0x61),PIXEL_COVERAGE(0x62),PIXEL_COVERAGE(0x63),
    PIXEL_COVERAGE(0x64),PIXEL_COVERAGE(0x65),PIXEL_COVERAGE(0x66),PIXEL_COVERAGE(0x67),
    PIXEL_COVERAGE(0x68),PIXEL_COVERAGE(0x69),PIXEL_COVERAGE(0x6a),PIXEL_COVERAGE(0x6b),
    PIXEL_COVERAGE(0x6c),PIXEL_COVERAGE(0x6d),PIXEL_COVERAGE(0x6e),PIXEL_COVERAGE(0x6f),
    PIXEL_COVERAGE(0x70),PIXEL_COVERAGE(0x71),PIXEL_COVERAGE(0x72),PIXEL_COVERAGE(0x73),
    PIXEL_COVERAGE(0x74),PIXEL_COVERAGE(0x75),PIXEL_COVERAGE(0x76),PIXEL_COVERAGE(0x77),
    PIXEL_COVERAGE(0x78),PIXEL_COVERAGE(0x79),PIXEL_COVERAGE(0x7a),PIXEL_COVERAGE(0x7b),
    PIXEL_COVERAGE(0x7c),PIXEL_COVERAGE(0x7d),PIXEL_COVERAGE(0x7e),PIXEL_COVERAGE(0x7f),
    PIXEL_COVERAGE(0x80),PIXEL_COVERAGE(0x81),PIXEL_COVERAGE(0x82),PIXEL_COVERAGE(0x83),
    PIXEL_COVERAGE(0x84),PIXEL_COVERAGE(0x85),PIXEL_COVERAGE(0x86),PIXEL_COVERAGE(0x87),
    PIXEL_COVERAGE(0x88),PIXEL_COVERAGE(0x89),PIXEL_COVERAGE(0x8a),PIXEL_COVERAGE(0x8b),
    PIXEL_COVERAGE(0x8c),PIXEL_COVERAGE(0x8d),PIXEL_COVERAGE(0x8e),PIXEL_COVERAGE(0x8f),
    PIXEL_COVERAGE(0x90),PIXEL_COVERAGE(0x91),PIXEL_COVERAGE(0x92),PIXEL_COVERAGE(0x93),
    PIXEL_COVERAGE(0x94),PIXEL_COVERAGE(0x95),PIXEL_COVERAGE(0x96),PIXEL_COVERAGE(0x97),
    PIXEL_COVERAGE(0x98),PIXEL_COVERAGE(0x99),PIXEL_COVERAGE(0x9a),PIXEL_COVERAGE(0x9b),
    PIXEL_COVERAGE(0x9c),PIXEL_COVERAGE(0x9d),PIXEL_COVERAGE(0x9e),PIXEL_COVERAGE(0x9f),
    PIXEL_COVERAGE(0xa0),PIXEL_COVERAGE(0xa1),PIXEL_COVERAGE(0xa2),PIXEL_COVERAGE(0xa3),
    PIXEL_COVERAGE(0xa4),PIXEL_COVERAGE(0xa5),PIXEL_COVERAGE(0xa6),PIXEL_COVERAGE(0xa7),
    PIXEL_COVERAGE(0xa8),PIXEL_COVERAGE(0xa9),PIXEL_COVERAGE(0xaa),PIXEL_COVERAGE(0xab),
    PIXEL_COVERAGE(0xac),PIXEL_COVERAGE(0xad),PIXEL_COVERAGE(0xae),PIXEL_COVERAGE(0xaf),
    PIXEL_COVERAGE(0xb0),PIXEL_COVERAGE(0xb1),PIXEL_COVERAGE(0xb2),PIXEL_COVERAGE(0xb3),
    PIXEL_COVERAGE(0xb4),PIXEL_COVERAGE(0xb5),PIXEL_COVERAGE(0xb6),PIXEL_COVERAGE(0xb7),
    PIXEL_COVERAGE(0xb8),PIXEL_COVERAGE(0xb9),PIXEL_COVERAGE(0xba),PIXEL_COVERAGE(0xbb),
    PIXEL_COVERAGE(0xbc),PIXEL_COVERAGE(0xbd),PIXEL_COVERAGE(0xbe),PIXEL_COVERAGE(0xbf),
    PIXEL_COVERAGE(0xc0),PIXEL_COVERAGE(0xc1),PIXEL_COVERAGE(0xc2),PIXEL_COVERAGE(0xc3),
    PIXEL_COVERAGE(0xc4),PIXEL_COVERAGE(0xc5),PIXEL_COVERAGE(0xc6),PIXEL_COVERAGE(0xc7),
    PIXEL_COVERAGE(0xc8),PIXEL_COVERAGE(0xc9),PIXEL_COVERAGE(0xca),PIXEL_COVERAGE(0xcb),
    PIXEL_COVERAGE(0xcc),PIXEL_COVERAGE(0xcd),PIXEL_COVERAGE(0xce),PIXEL_COVERAGE(0xcf),
    PIXEL_COVERAGE(0xd0),PIXEL_COVERAGE(0xd1),PIXEL_COVERAGE(0xd2),PIXEL_COVERAGE(0xd3),
    PIXEL_COVERAGE(0xd4),PIXEL_COVERAGE(0xd5),PIXEL_COVERAGE(0xd6),PIXEL_COVERAGE(0xd7),
    PIXEL_COVERAGE(0xd8),PIXEL_COVERAGE(0xd9),PIXEL_COVERAGE(0xda),PIXEL_COVERAGE(0xdb),
    PIXEL_COVERAGE(0xdc),PIXEL_COVERAGE(0xdd),PIXEL_COVERAGE(0xde),PIXEL_COVERAGE(0xdf),
    PIXEL_COVERAGE(0xe0),PIXEL_COVERAGE(0xe1),PIXEL_COVERAGE(0xe2),PIXEL_COVERAGE(0xe3),
    PIXEL_COVERAGE(0xe4),PIXEL_COVERAGE(0xe5),PIXEL_COVERAGE(0xe6),PIXEL_COVERAGE(0xe7),
    PIXEL_COVERAGE(0xe8),PIXEL_COVERAGE(0xe9),PIXEL_COVERAGE(0xea),PIXEL_COVERAGE(0xeb),
    PIXEL_COVERAGE(0xec),PIXEL_COVERAGE(0xed),PIXEL_COVERAGE(0xee),PIXEL_COVERAGE(0xef),
    PIXEL_COVERAGE(0xf0),PIXEL_COVERAGE(0xf1),PIXEL_COVERAGE(0xf2),PIXEL_COVERAGE(0xf3),
    PIXEL_COVERAGE(0xf4),PIXEL_COVERAGE(0xf5),PIXEL_COVERAGE(0xf6),PIXEL_COVERAGE(0xf7),
    PIXEL_COVERAGE(0xf8),PIXEL_COVERAGE(0xf9),PIXEL_COVERAGE(0xfa),PIXEL_COVERAGE(0xfb),
    PIXEL_COVERAGE(0xfc),PIXEL_COVERAGE(0xfd),PIXEL_COVERAGE(0xfe),PIXEL_COVERAGE(0xff)
};


#ifdef GATHER_STATISTICS
//! A class for storing rasterization statistics.
class PolygonRasterizationStatistics
{
public:
    //! Constructor.
    PolygonRasterizationStatistics()
    {
        reset();
    }

    //! Resets the statistics.
    void reset()
    {
        mFilledPixels = 0;
        mAntialiasPixels = 0;
        mOverdrawPixels = 0;
    }

    int mFilledPixels;
    int mAntialiasPixels;
    int mOverdrawPixels;
};
#endif

//! A struct for tracking the mask data of non-zero winding buffer.
typedef struct NonZeroMask
{
    SUBPIXEL_DATA mMask;
    NON_ZERO_MASK_DATA_UNIT mBuffer[SUBPIXEL_COUNT];
} NonZeroMask;

#define EVENODD_LINE_UNROLL_INIT() \
        int __evenodd_line_xp__;

#define EVENODD_LINE_UNROLL_0(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_0; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x01;

#define EVENODD_LINE_UNROLL_1(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_1; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x02;

#define EVENODD_LINE_UNROLL_2(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_2; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x04;

#define EVENODD_LINE_UNROLL_3(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_3; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x08;

#define EVENODD_LINE_UNROLL_4(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_4; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x10;

#define EVENODD_LINE_UNROLL_5(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_5; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x20;

#define EVENODD_LINE_UNROLL_6(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_6; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x40;

#define EVENODD_LINE_UNROLL_7(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_7; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x80;

#if SUBPIXEL_COUNT > 8

#define EVENODD_LINE_UNROLL_8(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_8; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0100;

#define EVENODD_LINE_UNROLL_9(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_9; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0200;

#define EVENODD_LINE_UNROLL_10(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_10; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0400;

#define EVENODD_LINE_UNROLL_11(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_11; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x0800;

#define EVENODD_LINE_UNROLL_12(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_12; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x1000;

#define EVENODD_LINE_UNROLL_13(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_13; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x2000;

#define EVENODD_LINE_UNROLL_14(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_14; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x4000;

#define EVENODD_LINE_UNROLL_15(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_15; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x8000;

#endif // !SUBPIXEL_COUNT > 8

#if SUBPIXEL_COUNT > 16

#define EVENODD_LINE_UNROLL_16(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_16; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00010000;

#define EVENODD_LINE_UNROLL_17(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_17; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00020000;

#define EVENODD_LINE_UNROLL_18(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_18; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00040000;

#define EVENODD_LINE_UNROLL_19(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_19; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00080000;

#define EVENODD_LINE_UNROLL_20(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_20; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00100000;

#define EVENODD_LINE_UNROLL_21(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_21; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00200000;

#define EVENODD_LINE_UNROLL_22(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_22; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00400000;

#define EVENODD_LINE_UNROLL_23(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_23; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x00800000;

#define EVENODD_LINE_UNROLL_24(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_24; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x01000000;

#define EVENODD_LINE_UNROLL_25(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_25; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x02000000;

#define EVENODD_LINE_UNROLL_26(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_26; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x04000000;

#define EVENODD_LINE_UNROLL_27(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_27; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x08000000;

#define EVENODD_LINE_UNROLL_28(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_28; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x10000000;

#define EVENODD_LINE_UNROLL_29(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_29; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x20000000;

#define EVENODD_LINE_UNROLL_30(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_30; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x40000000;

#define EVENODD_LINE_UNROLL_31(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_31; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x80000000;

#endif // !SUBPIXEL_COUNT > 16

// Macros for non-zero fill.

#define NONZERO_LINE_UNROLL_INIT() \
        int __nonzero_line_xp__;

#define NONZERO_LINE_UNROLL_0(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_0; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x01; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[0] += aWinding;

#define NONZERO_LINE_UNROLL_1(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_1; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x02; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[1] += aWinding;

#define NONZERO_LINE_UNROLL_2(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_2; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x04; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[2] += aWinding;

#define NONZERO_LINE_UNROLL_3(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_3; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x08; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[3] += aWinding;

#define NONZERO_LINE_UNROLL_4(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_4; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x10; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[4] += aWinding;

#define NONZERO_LINE_UNROLL_5(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_5; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x20; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[5] += aWinding;

#define NONZERO_LINE_UNROLL_6(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_6; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x40; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[6] += aWinding;

#define NONZERO_LINE_UNROLL_7(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_7; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x80; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[7] += aWinding;

#if SUBPIXEL_COUNT > 8

#define NONZERO_LINE_UNROLL_8(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_8; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0100; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[8] += aWinding;

#define NONZERO_LINE_UNROLL_9(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_9; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0200; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[9] += aWinding;

#define NONZERO_LINE_UNROLL_10(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_10; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0400; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[10] += aWinding;

#define NONZERO_LINE_UNROLL_11(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_11; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x0800; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[11] += aWinding;

#define NONZERO_LINE_UNROLL_12(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_12; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x1000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[12] += aWinding;

#define NONZERO_LINE_UNROLL_13(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_13; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x2000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[13] += aWinding;

#define NONZERO_LINE_UNROLL_14(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_14; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x4000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[14] += aWinding;

#define NONZERO_LINE_UNROLL_15(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_15; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x8000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[15] += aWinding;

#endif // !SUBPIXEL_COUNT > 8

#if SUBPIXEL_COUNT > 16

#define NONZERO_LINE_UNROLL_16(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_16; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00010000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[16] += aWinding;

#define NONZERO_LINE_UNROLL_17(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_17; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00020000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[17] += aWinding;

#define NONZERO_LINE_UNROLL_18(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_18; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00040000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[18] += aWinding;

#define NONZERO_LINE_UNROLL_19(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_19; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00080000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[19] += aWinding;

#define NONZERO_LINE_UNROLL_20(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_20; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00100000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[20] += aWinding;

#define NONZERO_LINE_UNROLL_21(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_21; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00200000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[21] += aWinding;

#define NONZERO_LINE_UNROLL_22(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_22; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00400000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[22] += aWinding;

#define NONZERO_LINE_UNROLL_23(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_23; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x00800000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[23] += aWinding;

#define NONZERO_LINE_UNROLL_24(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_24; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x01000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[24] += aWinding;

#define NONZERO_LINE_UNROLL_25(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_25; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x02000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[25] += aWinding;

#define NONZERO_LINE_UNROLL_26(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_26; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x04000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[26] += aWinding;

#define NONZERO_LINE_UNROLL_27(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_27; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x08000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[27] += aWinding;

#define NONZERO_LINE_UNROLL_28(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_28; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x10000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[28] += aWinding;

#define NONZERO_LINE_UNROLL_29(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_29; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x20000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[29] += aWinding;

#define NONZERO_LINE_UNROLL_30(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_30; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x40000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[30] += aWinding;

#define NONZERO_LINE_UNROLL_31(aWindingBuffer,aX,aSlope,aOffsets,aWinding) \
        __nonzero_line_xp__ = aX + SUBPIXEL_OFFSET_FIXED_31; \
        aX += aSlope; \
        __nonzero_line_xp__ = FIXED_TO_INT(__nonzero_line_xp__); \
        aWindingBuffer[__nonzero_line_xp__].mMask |= 0x80000000; \
        aWindingBuffer[__nonzero_line_xp__].mBuffer[31] += aWinding;

#endif // !SUBPIXEL_COUNT > 16

class PolygonWrapper
{
public:
    //! Virtual destructor.
    virtual ~PolygonWrapper() {};
};



class PolygonData
{
public:
    //! Constructor.
    PolygonData(const RATIONAL* const* aVertexData,
        const int* aVertexCounts,
        int aSubPolygonCount) 
        :
        mVertexData(aVertexData), 
        mVertexCounts(aVertexCounts), 
        mSubPolygonCount(aSubPolygonCount) 
    { }

    //! Returns the count of subpolygons
    inline int getSubPolygonCount() const
    {
        return mSubPolygonCount;
    }

    //! Returns a pointer to the vertex data array of a sub-polygon at given index.
    inline const RATIONAL* getVertexData(int aIndex) const
    {
        return mVertexData[aIndex];
    }

    //! Returns the vertex count of a sub-polygon at given index.
    inline int getVertexCount(int aIndex) const
    {
        return mVertexCounts[aIndex];
    }

protected:
    const RATIONAL* const* mVertexData;
    const int* mVertexCounts;
    int mSubPolygonCount;
};

//! Converts rational to int using direct asm.
/*! Typical conversions using _ftol are slow on Pentium 4, as _ftol sets the control
 *  state of the fpu unit. Also, we need a special rounding operation that rounds down,
 *  not towards zero. Therefore we use asm here. This requires that the rounding mode
 *  is set to round down by calling _controlfp(_MCW_RC,_RC_DOWN). The state of the
 *  control mode is thread specific, so other applications won't mix with this.
 */
inline int rationalToIntRoundDown(RATIONAL a)
{
#ifdef USE_ASM_OPTIMIZATIONS
    int i;
    _asm
    {
        fld a
        fistp i
    }
    return i;
#else
    return ((int)((a)+INT_TO_RATIONAL(1)) - 1);
#endif
}


//! The rational to fixed conversion is implemented with asm for performance reasons.
inline int rationalToFixed(RATIONAL a)
{
#ifdef USE_ASM_OPTIMIZATIONS
    int i;
    static const double scale = (double)(1 << FIXED_POINT_SHIFT);
    _asm
    {
        fld a
        fmul scale
        fistp i
    }
    return i;
#else
    return FLOAT_TO_FIXED(a);
#endif
}


//! Sets the FPU round down mode to correct value for the asm conversion routines.
inline void setRoundDownMode()
{
#ifdef USE_ASM_OPTIMIZATIONS
    _controlfp(_MCW_RC, _RC_DOWN);
#endif
}


// #define SUBPOLYGON_DEBUG

enum POLYGON_CLIP_FLAGS
{
    POLYGON_CLIP_NONE = 0x00,
    POLYGON_CLIP_LEFT = 0x01,
    POLYGON_CLIP_RIGHT = 0x02,
    POLYGON_CLIP_TOP = 0x04,
    POLYGON_CLIP_BOTTOM = 0x08,
};

#define CLIP_SUM_SHIFT 8
#define CLIP_UNION_SHIFT 4


 //! A polygon edge.
class PolygonEdge
{
public:
    int mFirstLine;
    int mLastLine;
    short mWinding;
    RATIONAL mX;
    RATIONAL mSlope;
};

//! A more optimized representation of a polygon edge.
class PolygonScanEdge
{
public:
    int mLastLine;
    int mFirstLine;
    short mWinding;
    FIXED_POINT mX;
    FIXED_POINT mSlope;
    FIXED_POINT mSlopeFix;
    class PolygonScanEdge* mNextEdge;
};

//! A class for holding processing data regarding the vertex.
class VertexData
{
public:
    VECTOR2D mPosition;
    int mClipFlags;
    int mLine;
};

//! A class for defining a clip rectangle.
class ClipRectangle
{
public:
    //! Constructor.
    ClipRectangle()
    {
        setClip(0, 0, 0, 0, 1);
    }

    //! Constructor with initialization.
    ClipRectangle(int aX, int aY, int aWidth, int aHeight, int aScale)
    {
        setClip(aX, aY, aWidth, aHeight, aScale);
    }

    //! Sets the clip values.
    void setClip(int aX, int aY, int aWidth, int aHeight, int aScale)
    {
        mMinXi = aX;
        mMinYi = aY * aScale;
        mMaxXi = aX + aWidth;
        mMaxYi = (aY + aHeight) * aScale;

        mXOffset = FLOAT_TO_RATIONAL(0.99f) / (RATIONAL)aScale;

        mMinXf = (RATIONAL)mMinXi + mXOffset;
        mMaxXf = (RATIONAL)mMaxXi - mXOffset;

        mMinYf = (RATIONAL)mMinYi;
        mMaxYf = (RATIONAL)mMaxYi;
    }

    //! Intersects the clip rectangle with another clip rectangle.
    void intersect(const ClipRectangle& aClipRectangle)
    {
        if (mMinXi < aClipRectangle.mMinXi)
            mMinXi = aClipRectangle.mMinXi;
        if (mMaxXi > aClipRectangle.mMaxXi)
            mMaxXi = aClipRectangle.mMaxXi;
        if (mMinXi > mMaxXi)
            mMinXi = mMaxXi;

        if (mMinYi < aClipRectangle.mMinYi)
            mMinYi = aClipRectangle.mMinYi;
        if (mMaxYi > aClipRectangle.mMaxYi)
            mMaxYi = aClipRectangle.mMaxYi;
        if (mMinYi > mMaxYi)
            mMinYi = mMaxYi;

        mMinXf = (RATIONAL)mMinXi + mXOffset;
        mMaxXf = (RATIONAL)mMaxXi - mXOffset;

        mMinYf = (RATIONAL)mMinYi;
        mMaxYf = (RATIONAL)mMaxYi;
    }

    //! Returns the minimum X as integer.
    inline int getMinXi() const { return mMinXi; }

    //! Returns the minimum Y as integer.
    inline int getMinYi() const { return mMinYi; }

    //! Returns the maximum X as integer.
    inline int getMaxXi() const { return mMaxXi; }

    //! Returns the maximum Y as integer.
    inline int getMaxYi() const { return mMaxYi; }

    //! Returns the minimum X as rational number (typically float).
    inline RATIONAL getMinXf() const { return mMinXf; }

    //! Returns the minimum Y as rational number (typically float).
    inline RATIONAL getMinYf() const { return mMinYf; }

    //! Returns the maximum X as rational number (typically float).
    inline RATIONAL getMaxXf() const { return mMaxXf; }

    //! Returns the maximum Y as rational number (typically float).
    inline RATIONAL getMaxYf() const { return mMaxYf; }

protected:
    int mMinXi;
    int mMinYi;
    int mMaxXi;
    int mMaxYi;
    RATIONAL mMinXf;
    RATIONAL mMinYf;
    RATIONAL mMaxXf;
    RATIONAL mMaxYf;
    RATIONAL mXOffset;
};

//! A class for handling sub-polygons. Each sub-polygon is a continuos, closed set of edges.
class SubPolygon
{
public:
    //! Constructor.
    /*! Note that the ownership of the vertex array is transferred.
     */
    SubPolygon(VECTOR2D* aVertices, int aVertexCount);

    //! Initializer.
    bool init();

    //! Destructor.
    ~SubPolygon();

    //! Returns the amount of vertices in the polygon.
    inline int getVertexCount() const
    {
        return mVertexCount;
    }

    //! Returns a vertex at given position.
    inline const VECTOR2D& getVertex(int aIndex) const
    {
        return mVertices[aIndex];
    }

    //! Calculates the edges of the polygon with transformation and clipping to aEdges array.
    /*! Note that this may return upto three times the amount of edges that aVertexCount defines,
     *  in the unlucky case where both left and right side get clipped for all edges.
     *  \param aFirstVertex the index for the first vertex.
     *  \param aVertexCount the amount of vertices to convert.
     *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
     *  \param aTransformation the transformation matrix for the polygon.
     *  \param aClipRectangle the clip rectangle.
     *  \return the amount of edges in the result.
     */
    int getEdges(int aFirstVertex, int aVertexCount, PolygonEdge* aEdges, const MATRIX2D& aTransformation, const ClipRectangle& aClipRectangle) const;

    //! Calculates the edges of the polygon with transformation and clipping to aEdges array.
    /*! Note that this may return upto three times the amount of edges that the polygon has vertices,
     *  in the unlucky case where both left and right side get clipped for all edges.
     *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
     *  \param aTransformation the transformation matrix for the polygon.
     *  \param aClipRectangle the clip rectangle.
     *  \return the amount of edges in the result.
     */
    int getScanEdges(PolygonScanEdge* aEdges, const MATRIX2D& aTransformation, const ClipRectangle& aClipRectangle) const;

protected:
    //! Calculates the clip flags for a point.
    inline int getClipFlags(const VECTOR2D& aPoint, const ClipRectangle& aClipRectangle) const;

    //! Creates a polygon edge between two vectors.
    /*! Clips the edge vertically to the clip rectangle. Returns true for edges that
     *  should be rendered, false for others.
     */
    bool getEdge(const VECTOR2D& aStart, const VECTOR2D& aEnd, PolygonEdge& aEdge, const ClipRectangle& aClipRectangle) const;

    //! Creates a vertical polygon edge between two y values.
    /*! Clips the edge vertically to the clip rectangle. Returns true for edges that
     *  should be rendered, false for others.
     */
    bool getVerticalEdge(RATIONAL aStartY, RATIONAL aEndY, RATIONAL aX, PolygonEdge& aEdge, const ClipRectangle& aClipRectangle) const;

    //! Returns the edge(s) between two vertices in aVertexData.
    inline int getScanEdge(VertexData* aVertexData, short aWinding, PolygonScanEdge* aEdges, const ClipRectangle& aClipRectangle) const;

#ifdef SUBPOLYGON_DEBUG
    //! A debug routine for checking that generated edge is valid.
    bool verifyEdge(const PolygonEdge& aEdge, const ClipRectangle& aClipRectangle) const;

    //! A debug routine for checking that generated edge is valid.
    bool verifyEdge(const PolygonScanEdge& aEdge, const ClipRectangle& aClipRectangle) const;
#endif

    int mVertexCount;
    VECTOR2D* mVertices;
    VertexData* mVertexData;
};
//! Constructor.
/*! Note that the ownership of the vertex array is transferred.
 */
SubPolygon::SubPolygon(VECTOR2D* aVertices, int aVertexCount)
{
    mVertices = aVertices;
    mVertexCount = aVertexCount;
    mVertexData = NULL;
}


//! Initializer.
bool SubPolygon::init()
{
    mVertexData = new VertexData[mVertexCount + 1];
    if (mVertexData == NULL)
        return false;
    return true;
}


//! Destructor.
SubPolygon::~SubPolygon()
{
    delete[] mVertices;
    delete[] mVertexData;
}


//! Calculates the edges of the polygon with transformation and clipping to aEdges array.
/*! \param aFirstVertex the index for the first vertex.
 *  \param aVertexCount the amount of vertices to convert.
 *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
 *  \param aTransformation the transformation matrix for the polygon.
 *  \param aClipRectangle the clip rectangle.
 *  \return the amount of edges in the result.
 */
int SubPolygon::getEdges(int aFirstVertex, int aVertexCount, PolygonEdge* aEdges, const MATRIX2D& aTransformation, const ClipRectangle& aClipRectangle) const
{
    int startIndex = aFirstVertex;
    int endIndex = startIndex + aVertexCount;
    if (endIndex > mVertexCount)
        endIndex = mVertexCount;

    VECTOR2D prevPosition = mVertices[startIndex];
    MATRIX2D_TRANSFORM(aTransformation, mVertices[startIndex], prevPosition);
    int prevClipFlags = getClipFlags(prevPosition, aClipRectangle);

    int edgeCount = 0;
    int n;
    for (n = startIndex; n < endIndex; n++)
    {
        VECTOR2D position = mVertices[(n + 1) % mVertexCount];
        MATRIX2D_TRANSFORM(aTransformation, mVertices[(n + 1) % mVertexCount], position);

        int clipFlags = getClipFlags(position, aClipRectangle);

        int clipSum = prevClipFlags | clipFlags;
        int clipUnion = prevClipFlags & clipFlags;

        // Skip all edges that are either completely outside at the top or at the bottom.
        if ((clipUnion & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM)) == 0)
        {
            if (clipUnion & POLYGON_CLIP_RIGHT)
            {
                // Both clip to right, edge is a vertical line on the right side
                if (getVerticalEdge(prevPosition.mY, position.mY, aClipRectangle.getMaxXf(), aEdges[edgeCount], aClipRectangle))
                {
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif
                    edgeCount++;
                }
            }
            else if (clipUnion & POLYGON_CLIP_LEFT)
            {
                // Both clip to left, edge is a vertical line on the left side
                if (getVerticalEdge(prevPosition.mY, position.mY, aClipRectangle.getMinXf(), aEdges[edgeCount], aClipRectangle))
                {
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif
                    edgeCount++;
                }
            }
            else if ((clipSum & (POLYGON_CLIP_RIGHT | POLYGON_CLIP_LEFT)) == 0)
            {
                // No clipping in the horizontal direction
                if (getEdge(prevPosition, position, aEdges[edgeCount], aClipRectangle))
                {
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif
                    edgeCount++;
                }
            }
            else
            {
                // Clips to left or right or both.
                VECTOR2D left, right;
                short swapWinding;
                if (VECTOR2D_GETX(position) < VECTOR2D_GETX(prevPosition))
                {
                    left = position;
                    right = prevPosition;
                    swapWinding = -1;
                }
                else
                {
                    left = prevPosition;
                    right = position;
                    swapWinding = 1;
                }

                RATIONAL slope = (VECTOR2D_GETY(right) - VECTOR2D_GETY(left)) /
                    (VECTOR2D_GETX(right) - VECTOR2D_GETX(left));

                if (clipSum & POLYGON_CLIP_RIGHT)
                {
                    // calculate new position for the right vertex
                    RATIONAL oldY = VECTOR2D_GETY(right);
                    RATIONAL maxX = aClipRectangle.getMaxXf();

                    VECTOR2D_SETY(right, VECTOR2D_GETY(left) + (maxX - VECTOR2D_GETX(left)) * slope);
                    VECTOR2D_SETX(right, maxX);

                    // add vertical edge for the overflowing part
                    if (getVerticalEdge(VECTOR2D_GETY(right), oldY, maxX, aEdges[edgeCount], aClipRectangle))
                    {
                        aEdges[edgeCount].mWinding *= swapWinding;
#ifdef SUBPOLYGON_DEBUG
                        DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif
                        edgeCount++;
                    }
                }

                if (clipSum & POLYGON_CLIP_LEFT)
                {
                    // calculate new position for the left vertex
                    RATIONAL oldY = VECTOR2D_GETY(left);
                    RATIONAL minX = aClipRectangle.getMinXf();

                    VECTOR2D_SETY(left, VECTOR2D_GETY(left) + (minX - VECTOR2D_GETX(left)) * slope);
                    VECTOR2D_SETX(left, minX);

                    // add vertical edge for the overflowing part
                    if (getVerticalEdge(oldY, VECTOR2D_GETY(left), minX, aEdges[edgeCount], aClipRectangle))
                    {
                        aEdges[edgeCount].mWinding *= swapWinding;
#ifdef SUBPOLYGON_DEBUG
                        DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif
                        edgeCount++;
                    }
                }

                if (getEdge(left, right, aEdges[edgeCount], aClipRectangle))
                {
                    aEdges[edgeCount].mWinding *= swapWinding;
#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif
                    edgeCount++;
                }
            }
        }

        prevClipFlags = clipFlags;
        prevPosition = position;
    }

    return edgeCount;
}

// Define for choosing 64-bit composite size instead of 32-bit
// This has speed effect only if the executable is built for 64-bit processor,
// otherwise 64-bit values are in fact handled as two 32-bit values.
// #define NON_ZERO_MASK_USE_UINT64_COMPOSITE


#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_UNIT unsigned long
#else
#define NON_ZERO_MASK_COMPOSITE_UNIT unsigned __int64
#endif


// Possible values for composite count
// data      composite     count
// 1         4             8            = 2
// 1         4             16           = 4
// 1         4             32           = 8
// 1         8             8            = 1
// 1         8             16           = 2
// 1         8             32           = 4
// 2         4             8            = 4
// 2         4             16           = 8
// 2         4             32           = 16
// 2         8             8            = 2
// 2         8             16           = 4
// 2         8             32           = 8

// These are the macros for packing the toggle bits from the composite values to lowest
// bits of the integer. These depend on the subpixel count, non-zero mask data size
// and the composite data type. The bits are packed down with the maximum efficiency,
// not caring about their order. This means that the scheme works only with unweighted
// sampling. If each sample has different weight, the weight calculations has to take
// the order into account.

#if SUBPIXEL_COUNT == 8
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
        // byte per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 2
        // long per composite
        // pattern is ......AB......CD......EF......GH
        // a |= a >> 14;
        // pattern is ......AB......CD....ABEF....CDGH
        // a |= a >> 4;
        // pattern is ......AB......CD....ABEFABEFCDGH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 14; (a) |= (a) >> 4;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 1
        // uint64 per composite
        // pattern is .......A.......B.......C.......D.......E.......F.......G.......H
        // a |= a >> 31;
        // pattern is .......A.......B.......C.......D......AE......BF......CG......DH
        // a |= a >> 14;
        // pattern is .......A.......B.......C.......D......AE......BF....AECG....BFDH
        // a |= a >> 4;
        // pattern is .......A.......B.......C.......D......AE......BF....AECGAECGBFDH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 31; (a) |= (a) >> 14; (a) |= (a) >> 4;
#endif
#else
        // short per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // long per composite
        // pattern is ............ABCD............EFGH
        // a |= a >> 12;
        // pattern is ............ABCD........ABCDEFGH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 12;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 2
        // uint64 per composite
        // pattern is ..............AB..............CD..............EF..............GH
        // a |= a >> 30;
        // pattern is ..............AB..............CD............ABEF............CDGH
        // a |= a >> 12;
        // pattern is ..............AB..............CD............ABEF........ABEFCDGH
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 30; (a) |= (a) >> 12;
#endif
#endif
#endif

#if SUBPIXEL_COUNT == 16
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
        // byte per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // long per composite
        // pattern is ....ABCD....EFGH....IJKL....MNOP
        // a |= a >> 12;
        // pattern is ....ABCD....EFGHABCDIJKLEFGHMNOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 12;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 2
        // uint64 per composite
        // pattern is ......AB......CD......EF......GH......IJ......KL......MN......OP
        // a |= a >> 30;
        // pattern is ......AB......CD......EF......GH....ABIJ....CDKL....EFMN....GHOP
        // a |= a >> 12;
        // pattern is ......AB......CD......EF......GH....ABIJ....CDKLABIJEFMNCDKLGHOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 30; (a) |= (a) >> 12;
#endif
#else
        // short per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 8
        // long per composite
        // pattern is ........ABCDEFGH........IJKLMNOP
        // a |= a >> 8;
        // pattern is ........ABCDEFGHABCDEFGHIJKLMNOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 8;
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // uint64 per composite
        // pattern is ............ABCD............EFGH............IJKL............MNOP
        // a |= a >> 28;
        // pattern is ............ABCD............EFGH........ABCDIJKL........EFGHMNOP
        // a |= a >> 8;
        // pattern is ............ABCD............EFGH........ABCDIJKLABCDIJKLEFGHMNOP
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 28; (a) |= (a) >> 8;
#endif
#endif
#endif

#if SUBPIXEL_COUNT == 32
#ifndef NON_ZERO_MASK_USE_SHORT_DATA
        // byte per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 8
        // long per composite
        // pattern is ABCDEFGHIJKLMNOPQRSTUVWXYZ123456
#define NON_ZERO_MASK_COMPOSE_MASK(a)
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 4
        // uint64 per composite
        // pattern is ....ABCD....EFGH....IJKL....MNOP....QRST....UVWX....YZ12....3456
        // tmask |= tmask >> 28;
        // pattern is ....ABCD....EFGH....IJKL....MNOPABCDQRSTEFGHUVWXIJKLYZ12MNOP3456
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 28;
#endif
#else
        // short per unit
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
#define NON_ZERO_MASK_COMPOSITE_COUNT 16
        // long per composite
        // pattern is ABCDEFGHIJKLMNOPQRSTUVWXYZ123456
#define NON_ZERO_MASK_COMPOSE_MASK(a)
#else
#define NON_ZERO_MASK_COMPOSITE_COUNT 8
        // uint64 per composite
        // pattern is ........ABCDEFGH........IJKLMNOP........QRSTUVWX........YZ123456
tmask |= tmask >> 24;
// pattern is ........ABCDEFGH........IJKLMNOPABCDEFGHQRSTUVWXIJKLMNOPYZ123456
#define NON_ZERO_MASK_COMPOSE_MASK(a) (a) |= (a) >> 24;
#endif
#endif
#endif

//! A structure for containing the composite mask data, definable to various sizes.
typedef struct NonZeroMaskCompositeData
{
    NON_ZERO_MASK_COMPOSITE_UNIT mValue0;
#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
    NON_ZERO_MASK_COMPOSITE_UNIT mValue1;
#endif
#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
    NON_ZERO_MASK_COMPOSITE_UNIT mValue2;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue3;
#endif
#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
    NON_ZERO_MASK_COMPOSITE_UNIT mValue4;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue5;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue6;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue7;
#endif
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
    NON_ZERO_MASK_COMPOSITE_UNIT mValue8;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue9;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue10;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue11;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue12;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue13;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue14;
    NON_ZERO_MASK_COMPOSITE_UNIT mValue15;
#endif
} NonZeroMaskCompositeData;


#ifndef NON_ZERO_MASK_USE_SHORT_DATA
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const unsigned long NonZeroMaskHighBitOff = 0x7f7f7f7f;
static const unsigned long NonZeroMaskLowBitsOff = 0x80808080;
#else // NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const unsigned __int64 NonZeroMaskHighBitOff = 0x7f7f7f7f7f7f7f7f;
static const unsigned __int64 NonZeroMaskLowBitsOff = 0x8080808080808080;
#endif // NON_ZERO_MASK_USE_UINT64_COMPOSITE
#else // NON_ZERO_MASK_USE_SHORT_DATA
#ifndef NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const unsigned long NonZeroMaskHighBitOff = 0x7fff7fff;
static const unsigned long NonZeroMaskLowBitsOff = 0x80008000;
#else // NON_ZERO_MASK_USE_UINT64_COMPOSITE
static const unsigned __int64 NonZeroMaskHighBitOff = 0x7fff7fff7fff7fff;
static const unsigned __int64 NonZeroMaskLowBitsOff = 0x8000800080008000;
#endif // NON_ZERO_MASK_USE_UINT64_COMPOSITE
#endif // NON_ZERO_MASK_USE_SHORT_DATA


//! An implementation for tracking the mask data of non-zero winding buffer.
/*! It builds the masks using parallelism within a command - for instance 32-bit integer
 *  can be used for performing 4 8-bit operations.
 */
class NonZeroMaskC
{
public:
    static inline void reset(NonZeroMask& aSource, NonZeroMask& aDestination)
    {
        NonZeroMaskCompositeData& sourceComposite = *((NonZeroMaskCompositeData*)aSource.mBuffer);
        NonZeroMaskCompositeData& destinationComposite = *((NonZeroMaskCompositeData*)aDestination.mBuffer);

        register NON_ZERO_MASK_COMPOSITE_UNIT tmask = 0;
        register NON_ZERO_MASK_COMPOSITE_UNIT t;

        t = sourceComposite.mValue0;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue0 = t;
        sourceComposite.mValue0 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 1);
        tmask |= t;

#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
        t = sourceComposite.mValue1;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue1 = t;
        sourceComposite.mValue1 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 2);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
        t = sourceComposite.mValue2;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue2 = t;
        sourceComposite.mValue2 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 3);
        tmask |= t;

        t = sourceComposite.mValue3;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue3 = t;
        sourceComposite.mValue3 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 4);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
        t = sourceComposite.mValue4;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue4 = t;
        sourceComposite.mValue4 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 5);
        tmask |= t;

        t = sourceComposite.mValue5;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue5 = t;
        sourceComposite.mValue5 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 6);
        tmask |= t;

        t = sourceComposite.mValue6;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue6 = t;
        sourceComposite.mValue6 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 7);
        tmask |= t;

        t = sourceComposite.mValue7;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue7 = t;
        sourceComposite.mValue7 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 8
        t >>= (NON_ZERO_MASK_DATA_BITS - 8);
#endif
        tmask |= t;
#endif

        // As indicated by the table earlier, this case can be reached in a single
        // case: 2 bytes per unit, 4 bytes in composite, 32 bits in mask
        // This means that NON_ZERO_MASK_DATA_BITS is always 16.
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
        t = sourceComposite.mValue8;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue8 = t;
        sourceComposite.mValue8 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 9);
        tmask |= t;

        t = sourceComposite.mValue9;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue9 = t;
        sourceComposite.mValue9 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 10);
        tmask |= t;

        t = sourceComposite.mValue10;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue10 = t;
        sourceComposite.mValue10 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 11);
        tmask |= t;

        t = sourceComposite.mValue11;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue11 = t;
        sourceComposite.mValue11 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 12);
        tmask |= t;

        t = sourceComposite.mValue12;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue12 = t;
        sourceComposite.mValue12 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 13);
        tmask |= t;

        t = sourceComposite.mValue13;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue13 = t;
        sourceComposite.mValue13 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 14);
        tmask |= t;

        t = sourceComposite.mValue14;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue14 = t;
        sourceComposite.mValue14 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 15);
        tmask |= t;

        t = sourceComposite.mValue15;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue15 = t;
        sourceComposite.mValue15 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 16
        t >>= (NON_ZERO_MASK_DATA_BITS - 16);
#endif
        tmask |= t;
#endif

        NON_ZERO_MASK_COMPOSE_MASK(tmask);

        aDestination.mMask = (SUBPIXEL_DATA)tmask;
        aSource.mMask = 0;
    }


    static inline void init(NonZeroMask& aSource, NonZeroMask& aDestination)
    {
        NonZeroMaskCompositeData& sourceComposite = *((NonZeroMaskCompositeData*)aSource.mBuffer);
        NonZeroMaskCompositeData& destinationComposite = *((NonZeroMaskCompositeData*)aDestination.mBuffer);

        register NON_ZERO_MASK_COMPOSITE_UNIT tmask = 0;
        register NON_ZERO_MASK_COMPOSITE_UNIT t;

        t = sourceComposite.mValue0;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue0 = t;
        sourceComposite.mValue0 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 1);
        tmask |= t;

#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
        t = sourceComposite.mValue1;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue1 = t;
        sourceComposite.mValue1 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 2);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
        t = sourceComposite.mValue2;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue2 = t;
        sourceComposite.mValue2 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 3);
        tmask |= t;

        t = sourceComposite.mValue3;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue3 = t;
        sourceComposite.mValue3 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 4);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
        t = sourceComposite.mValue4;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue4 = t;
        sourceComposite.mValue4 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 5);
        tmask |= t;

        t = sourceComposite.mValue5;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue5 = t;
        sourceComposite.mValue5 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 6);
        tmask |= t;

        t = sourceComposite.mValue6;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue6 = t;
        sourceComposite.mValue6 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 7);
        tmask |= t;

        t = sourceComposite.mValue7;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue7 = t;
        sourceComposite.mValue7 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 8
        t >>= (NON_ZERO_MASK_DATA_BITS - 8);
#endif
        tmask |= t;
#endif

        // As indicated by the table earlier, this case can be reached in a single
        // case: 2 bytes per unit, 4 bytes in composite, 32 bits in mask
        // This means that NON_ZERO_MASK_DATA_BITS is always 16.
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
        t = sourceComposite.mValue8;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue8 = t;
        sourceComposite.mValue8 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 9);
        tmask |= t;

        t = sourceComposite.mValue9;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue9 = t;
        sourceComposite.mValue9 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 10);
        tmask |= t;

        t = sourceComposite.mValue10;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue10 = t;
        sourceComposite.mValue10 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 11);
        tmask |= t;

        t = sourceComposite.mValue11;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue11 = t;
        sourceComposite.mValue11 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 12);
        tmask |= t;

        t = sourceComposite.mValue12;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue12 = t;
        sourceComposite.mValue12 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 13);
        tmask |= t;

        t = sourceComposite.mValue13;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue13 = t;
        sourceComposite.mValue13 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 14);
        tmask |= t;

        t = sourceComposite.mValue14;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue14 = t;
        sourceComposite.mValue14 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 15);
        tmask |= t;

        t = sourceComposite.mValue15;
        t &= NonZeroMaskHighBitOff;
        destinationComposite.mValue15 = t;
        sourceComposite.mValue15 = 0;
        t += NonZeroMaskHighBitOff;
        t &= NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 16
        t >>= (NON_ZERO_MASK_DATA_BITS - 16);
#endif
        tmask |= t;
#endif

        NON_ZERO_MASK_COMPOSE_MASK(tmask);

        aDestination.mMask = (SUBPIXEL_DATA)tmask;
        aSource.mMask = 0;
    }


    static inline void apply(NonZeroMask& aSource, NonZeroMask& aDestination)
    {
        NonZeroMaskCompositeData& sourceComposite = *((NonZeroMaskCompositeData*)aSource.mBuffer);
        NonZeroMaskCompositeData& destinationComposite = *((NonZeroMaskCompositeData*)aDestination.mBuffer);

        register NON_ZERO_MASK_COMPOSITE_UNIT tmask = 0;
        register NON_ZERO_MASK_COMPOSITE_UNIT t, p;

        t = destinationComposite.mValue0 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue0 + (sourceComposite.mValue0 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue0 = 0;
        destinationComposite.mValue0 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 1);
        tmask |= t;

#if NON_ZERO_MASK_COMPOSITE_COUNT > 1
        t = destinationComposite.mValue1 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue1 + (sourceComposite.mValue1 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue1 = 0;
        destinationComposite.mValue1 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 2);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 2
        t = destinationComposite.mValue2 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue2 + (sourceComposite.mValue2 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue2 = 0;
        destinationComposite.mValue2 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 3);
        tmask |= t;

        t = destinationComposite.mValue3 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue3 + (sourceComposite.mValue3 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue3 = 0;
        destinationComposite.mValue3 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 4);
        tmask |= t;
#endif

#if NON_ZERO_MASK_COMPOSITE_COUNT > 4
        t = destinationComposite.mValue4 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue4 + (sourceComposite.mValue4 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue4 = 0;
        destinationComposite.mValue4 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 5);
        tmask |= t;

        t = destinationComposite.mValue5 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue5 + (sourceComposite.mValue5 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue5 = 0;
        destinationComposite.mValue5 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 6);
        tmask |= t;

        t = destinationComposite.mValue6 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue6 + (sourceComposite.mValue6 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue6 = 0;
        destinationComposite.mValue6 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 7);
        tmask |= t;

        t = destinationComposite.mValue7 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue7 + (sourceComposite.mValue7 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue7 = 0;
        destinationComposite.mValue7 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 8
        t >>= (NON_ZERO_MASK_DATA_BITS - 8);
#endif
        tmask |= t;
#endif

        // As indicated by the table earlier, this case can be reached in a single
        // case: 2 bytes per unit, 4 bytes in composite, 32 bits in mask
        // This means that NON_ZERO_MASK_DATA_BITS is always 16 below.
#if NON_ZERO_MASK_COMPOSITE_COUNT > 8
        t = destinationComposite.mValue8 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue8 + (sourceComposite.mValue8 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue8 = 0;
        destinationComposite.mValue8 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 9);
        tmask |= t;

        t = destinationComposite.mValue9 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue9 + (sourceComposite.mValue9 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue9 = 0;
        destinationComposite.mValue9 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 10);
        tmask |= t;

        t = destinationComposite.mValue10 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue10 + (sourceComposite.mValue10 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue10 = 0;
        destinationComposite.mValue10 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 11);
        tmask |= t;

        t = destinationComposite.mValue11 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue11 + (sourceComposite.mValue11 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue11 = 0;
        destinationComposite.mValue11 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 12);
        tmask |= t;

        t = destinationComposite.mValue12 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue12 + (sourceComposite.mValue12 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue12 = 0;
        destinationComposite.mValue12 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 13);
        tmask |= t;

        t = destinationComposite.mValue13 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue13 + (sourceComposite.mValue13 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue13 = 0;
        destinationComposite.mValue13 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 14);
        tmask |= t;

        t = destinationComposite.mValue14 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue14 + (sourceComposite.mValue14 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue14 = 0;
        destinationComposite.mValue14 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
        t >>= (NON_ZERO_MASK_DATA_BITS - 15);
        tmask |= t;

        t = destinationComposite.mValue15 + NonZeroMaskHighBitOff;
        p = (destinationComposite.mValue15 + (sourceComposite.mValue15 & NonZeroMaskHighBitOff)) & NonZeroMaskHighBitOff;
        sourceComposite.mValue15 = 0;
        destinationComposite.mValue15 = p;
        p += NonZeroMaskHighBitOff;
        t = (t ^ p) & NonZeroMaskLowBitsOff;
#if NON_ZERO_MASK_DATA_BITS != 16
        t >>= (NON_ZERO_MASK_DATA_BITS - 16);
#endif
        tmask |= t;
#endif

        NON_ZERO_MASK_COMPOSE_MASK(tmask);

        aDestination.mMask ^= (SUBPIXEL_DATA)tmask;
        aSource.mMask = 0;
    }
};


//! Calculates the clip flags for a point.
inline int SubPolygon::getClipFlags(const VECTOR2D& aPoint, const ClipRectangle& aClipRectangle) const
{
    int flags = POLYGON_CLIP_NONE;

    if (VECTOR2D_GETX(aPoint) < aClipRectangle.getMinXf())
        flags |= POLYGON_CLIP_LEFT;
    else if (VECTOR2D_GETX(aPoint) >= aClipRectangle.getMaxXf())
        flags |= POLYGON_CLIP_RIGHT;

    if (VECTOR2D_GETY(aPoint) < aClipRectangle.getMinYf())
        flags |= POLYGON_CLIP_TOP;
    else if (VECTOR2D_GETY(aPoint) >= aClipRectangle.getMaxYf())
        flags |= POLYGON_CLIP_BOTTOM;

    return flags;
}


//! Creates a polygon edge between two vectors.
/*! Clips the edge vertically to the clip rectangle. Returns true for edges that
 *  should be rendered, false for others.
 */
bool SubPolygon::getEdge(const VECTOR2D& aStart, const VECTOR2D& aEnd, PolygonEdge& aEdge, const ClipRectangle& aClipRectangle) const
{
    RATIONAL startX, startY, endX, endY;
    short winding;

    if (VECTOR2D_GETY(aStart) <= VECTOR2D_GETY(aEnd))
    {
        startX = VECTOR2D_GETX(aStart);
        startY = VECTOR2D_GETY(aStart);
        endX = VECTOR2D_GETX(aEnd);
        endY = VECTOR2D_GETY(aEnd);
        winding = 1;
    }
    else
    {
        startX = VECTOR2D_GETX(aEnd);
        startY = VECTOR2D_GETY(aEnd);
        endX = VECTOR2D_GETX(aStart);
        endY = VECTOR2D_GETY(aStart);
        winding = -1;
    }

    // Essentially, firstLine is floor(startY + 1) and lastLine is floor(endY).
    // These are refactored to integer casts in order to avoid function
    // calls. The difference with integer cast is that numbers are always
    // rounded towards zero. Since values smaller than zero get clipped away,
    // only coordinates between 0 and -1 require greater attention as they
    // also round to zero. The problems in this range can be avoided by
    // adding one to the values before conversion and subtracting after it.

    int firstLine = rationalToIntRoundDown(startY) + 1;
    int lastLine = rationalToIntRoundDown(endY);

    int minClip = aClipRectangle.getMinYi();
    int maxClip = aClipRectangle.getMaxYi();

    // If start and end are on the same line, the edge doesn't cross
    // any lines and thus can be ignored.
    // If the end is smaller than the first line, edge is out.
    // If the start is larger than the last line, edge is out.
    if (firstLine > lastLine ||
        lastLine < minClip ||
        firstLine >= maxClip)
        return false;

    // Adjust the start based on the target.
    if (firstLine < minClip)
        firstLine = minClip;

    if (lastLine >= maxClip)
        lastLine = maxClip - 1;

    aEdge.mSlope = (endX - startX) / (endY - startY);
    aEdge.mX = startX + ((RATIONAL)firstLine - startY) * aEdge.mSlope;
    aEdge.mWinding = winding;
    aEdge.mFirstLine = firstLine;
    aEdge.mLastLine = lastLine;

    return true;
}


//! Creates a vertical polygon edge between two y values.
/*! Clips the edge vertically to the clip rectangle. Returns true for edges that
 *  should be rendered, false for others.
 */
bool SubPolygon::getVerticalEdge(RATIONAL aStartY, RATIONAL aEndY, RATIONAL aX, PolygonEdge& aEdge, const ClipRectangle& aClipRectangle) const
{
    RATIONAL start, end;
    short winding;
    if (aStartY < aEndY)
    {
        start = aStartY;
        end = aEndY;
        winding = 1;
    }
    else
    {
        start = aEndY;
        end = aStartY;
        winding = -1;
    }

    int firstLine = rationalToIntRoundDown(start) + 1;
    int lastLine = rationalToIntRoundDown(end);

    int minClip = aClipRectangle.getMinYi();
    int maxClip = aClipRectangle.getMaxYi();

    // If start and end are on the same line, the edge doesn't cross
    // any lines and thus can be ignored.
    // If the end is smaller than the first line, edge is out.
    // If the start is larger than the last line, edge is out.
    if (firstLine > lastLine ||
        lastLine < minClip ||
        firstLine >= maxClip)
        return false;

    // Adjust the start based on the clip rect.
    if (firstLine < minClip)
        firstLine = minClip;

    if (lastLine >= maxClip)
        lastLine = maxClip - 1;

    aEdge.mSlope = INT_TO_RATIONAL(0);
    aEdge.mX = aX;
    aEdge.mWinding = winding;
    aEdge.mFirstLine = firstLine;
    aEdge.mLastLine = lastLine;

    return true;
}


//! Calculates the edges of the polygon with transformation and clipping to aEdges array.
/*! Note that this may return upto three times the amount of edges that the polygon has vertices,
 *  in the unlucky case where both left and right side get clipped for all edges.
 *  \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
 *  \param aTransformation the transformation matrix for the polygon.
 *  \param aClipRectangle the clip rectangle.
 *  \return the amount of edges in the result.
 */
int SubPolygon::getScanEdges(PolygonScanEdge* aEdges, const MATRIX2D& aTransformation, const ClipRectangle& aClipRectangle) const
{
    int n;
    for (n = 0; n < mVertexCount; n++)
    {
        // Transform all vertices.
        MATRIX2D_TRANSFORM(aTransformation, mVertices[n], mVertexData[n].mPosition);
    }

    for (n = 0; n < mVertexCount; n++)
    {
        // Calculate clip flags for all vertices.
        mVertexData[n].mClipFlags = getClipFlags(mVertexData[n].mPosition, aClipRectangle);

        // Calculate line of the vertex. If the vertex is clipped by top or bottom, the line
        // is determined by the clip rectangle.
        if (mVertexData[n].mClipFlags & POLYGON_CLIP_TOP)
        {
            mVertexData[n].mLine = aClipRectangle.getMinYi();
        }
        else if (mVertexData[n].mClipFlags & POLYGON_CLIP_BOTTOM)
        {
            mVertexData[n].mLine = aClipRectangle.getMaxYi() - 1;
        }
        else
        {
            mVertexData[n].mLine = rationalToIntRoundDown(VECTOR2D_GETY(mVertexData[n].mPosition));
        }
        mVertexData[n].mLine = rationalToIntRoundDown(VECTOR2D_GETY(mVertexData[n].mPosition));
    }

    // Copy the data from 0 to the last entry to make the data to loop.
    mVertexData[mVertexCount] = mVertexData[0];

    // Transform the first vertex; store.
    // Process mVertexCount - 1 times, next is n+1
    // copy the first vertex to
    // Process 1 time, next is n

    int edgeCount = 0;
    for (n = 0; n < mVertexCount; n++)
    {
        int clipSum = mVertexData[n].mClipFlags | mVertexData[n + 1].mClipFlags;
        int clipUnion = mVertexData[n].mClipFlags & mVertexData[n + 1].mClipFlags;

        if ((clipUnion & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM)) == 0 &&
            mVertexData[n].mLine != mVertexData[n + 1].mLine)
        {
            int startIndex, endIndex;
            short winding;
            if (VECTOR2D_GETY(mVertexData[n].mPosition) < VECTOR2D_GETY(mVertexData[n + 1].mPosition))
            {
                startIndex = n;
                endIndex = n + 1;
                winding = 1;
            }
            else
            {
                startIndex = n + 1;
                endIndex = n;
                winding = -1;
            }

            int firstLine = mVertexData[startIndex].mLine + 1;
            int lastLine = mVertexData[endIndex].mLine;

            if (clipUnion & POLYGON_CLIP_RIGHT)
            {
                // Both clip to right, edge is a vertical line on the right side
                aEdges[edgeCount].mFirstLine = firstLine;
                aEdges[edgeCount].mLastLine = lastLine;
                aEdges[edgeCount].mWinding = winding;
                aEdges[edgeCount].mX = rationalToFixed(aClipRectangle.getMaxXf());
                aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);

#ifdef SUBPOLYGON_DEBUG
                DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif

                edgeCount++;
            }
            else if (clipUnion & POLYGON_CLIP_LEFT)
            {
                // Both clip to left, edge is a vertical line on the left side
                aEdges[edgeCount].mFirstLine = firstLine;
                aEdges[edgeCount].mLastLine = lastLine;
                aEdges[edgeCount].mWinding = winding;
                aEdges[edgeCount].mX = rationalToFixed(aClipRectangle.getMinXf());
                aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);

#ifdef SUBPOLYGON_DEBUG
                DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif

                edgeCount++;
            }
            else if ((clipSum & (POLYGON_CLIP_RIGHT | POLYGON_CLIP_LEFT)) == 0)
            {
                // No clipping in the horizontal direction
                RATIONAL slope = (VECTOR2D_GETX(mVertexData[endIndex].mPosition) -
                    VECTOR2D_GETX(mVertexData[startIndex].mPosition)) /
                    (VECTOR2D_GETY(mVertexData[endIndex].mPosition) -
                        VECTOR2D_GETY(mVertexData[startIndex].mPosition));

                // If there is vertical clip (for the top) it will be processed here. The calculation
                // should be done for all non-clipping edges as well to determine the accurate position
                // where the edge crosses the first scanline.
                RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                    ((RATIONAL)firstLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                aEdges[edgeCount].mFirstLine = firstLine;
                aEdges[edgeCount].mLastLine = lastLine;
                aEdges[edgeCount].mWinding = winding;
                aEdges[edgeCount].mX = rationalToFixed(startx);
                aEdges[edgeCount].mSlope = rationalToFixed(slope);

                if (lastLine - firstLine >= SLOPE_FIX_STEP)
                {
                    aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                        (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                }
                else
                {
                    aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                }

#ifdef SUBPOLYGON_DEBUG
                DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif

                edgeCount++;
            }
            else
            {
                // Clips to left or right or both.
                RATIONAL slope = (VECTOR2D_GETX(mVertexData[endIndex].mPosition) -
                    VECTOR2D_GETX(mVertexData[startIndex].mPosition)) /
                    (VECTOR2D_GETY(mVertexData[endIndex].mPosition) -
                        VECTOR2D_GETY(mVertexData[startIndex].mPosition));

                // The edge may clip to both left and right.
                // The clip results in one or two new vertices, and one to three segments.
                // The rounding for scanlines may produce a result where any of the segments is
                // ignored.

                // The start is always above the end. Calculate the clip positions to clipVertices.
                // It is possible that only one of the vertices exist. This will be detected from the
                // clip flags of the vertex later, so they are initialized here.
                VertexData clipVertices[2];

                if (VECTOR2D_GETX(mVertexData[startIndex].mPosition) <
                    VECTOR2D_GETX(mVertexData[endIndex].mPosition))
                {
                    VECTOR2D_SETX(clipVertices[0].mPosition, aClipRectangle.getMinXf());
                    VECTOR2D_SETX(clipVertices[1].mPosition, aClipRectangle.getMaxXf());
                    clipVertices[0].mClipFlags = POLYGON_CLIP_LEFT;
                    clipVertices[1].mClipFlags = POLYGON_CLIP_RIGHT;
                }
                else
                {
                    VECTOR2D_SETX(clipVertices[0].mPosition, aClipRectangle.getMaxXf());
                    VECTOR2D_SETX(clipVertices[1].mPosition, aClipRectangle.getMinXf());
                    clipVertices[0].mClipFlags = POLYGON_CLIP_RIGHT;
                    clipVertices[1].mClipFlags = POLYGON_CLIP_LEFT;
                }

                int p;
                for (p = 0; p < 2; p++)
                {
                    // Check if either of the vertices crosses the edge marked for the clip vertex
                    if (clipSum & clipVertices[p].mClipFlags)
                    {
                        // The the vertex is required, calculate it.
                        VECTOR2D_SETY(clipVertices[p].mPosition, VECTOR2D_GETY(mVertexData[startIndex].mPosition) +
                            (VECTOR2D_GETX(clipVertices[p].mPosition) -
                                VECTOR2D_GETX(mVertexData[startIndex].mPosition)) / slope);

                        // If there is clipping in the vertical direction, the new vertex may be clipped.
                        if (clipSum & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM))
                        {
                            if (VECTOR2D_GETY(clipVertices[p].mPosition) < aClipRectangle.getMinYf())
                            {
                                clipVertices[p].mClipFlags = POLYGON_CLIP_TOP;
                                clipVertices[p].mLine = aClipRectangle.getMinYi();
                            }
                            else if (VECTOR2D_GETY(clipVertices[p].mPosition) > aClipRectangle.getMaxYf())
                            {
                                clipVertices[p].mClipFlags = POLYGON_CLIP_BOTTOM;
                                clipVertices[p].mLine = aClipRectangle.getMaxYi() - 1;
                            }
                            else
                            {
                                clipVertices[p].mClipFlags = 0;
                                clipVertices[p].mLine = rationalToIntRoundDown(VECTOR2D_GETY(clipVertices[p].mPosition));
                            }
                        }
                        else
                        {
                            clipVertices[p].mClipFlags = 0;
                            clipVertices[p].mLine = rationalToIntRoundDown(VECTOR2D_GETY(clipVertices[p].mPosition));
                        }
                    }
                }

                // Now there are three or four vertices, in the top-to-bottom order of start, clip0, clip1,
                // end. What kind of edges are required for connecting these can be determined from the
                // clip flags.
                // -if clip vertex has horizontal clip flags, it doesn't exist. No edge is generated.
                // -if start vertex or end vertex has horizontal clip flag, the edge to/from the clip vertex is vertical
                // -if the line of two vertices is the same, the edge is not generated, since the edge doesn't
                //  cross any scanlines.

                // The alternative patterns are:
                // start - clip0 - clip1 - end
                // start - clip0 - end
                // start - clip1 - end

                int topClipIndex;
                int bottomClipIndex;
                if (((clipVertices[0].mClipFlags | clipVertices[1].mClipFlags) &
                    (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT)) == 0)
                {
                    // Both sides are clipped, the order is start-clip0-clip1-end
                    topClipIndex = 0;
                    bottomClipIndex = 1;

                    // Add the edge from clip0 to clip1
                    // Check that the line is different for the vertices.
                    if (clipVertices[0].mLine != clipVertices[1].mLine)
                    {
                        int firstClipLine = clipVertices[0].mLine + 1;

                        RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                            ((RATIONAL)firstClipLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                        aEdges[edgeCount].mX = rationalToFixed(startx);
                        aEdges[edgeCount].mSlope = rationalToFixed(slope);
                        aEdges[edgeCount].mFirstLine = firstClipLine;
                        aEdges[edgeCount].mLastLine = clipVertices[1].mLine;
                        aEdges[edgeCount].mWinding = winding;

                        if (aEdges[edgeCount].mLastLine - aEdges[edgeCount].mFirstLine >= SLOPE_FIX_STEP)
                        {
                            aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                        }

#ifdef SUBPOLYGON_DEBUG
                        DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif

                        edgeCount++;
                    }
                }
                else
                {
                    // Clip at either side, check which side. The clip flag is on for the vertex
                    // that doesn't exist, i.e. has not been clipped to be inside the rect.
                    if (clipVertices[0].mClipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        topClipIndex = 1;
                        bottomClipIndex = 1;
                    }
                    else
                    {
                        topClipIndex = 0;
                        bottomClipIndex = 0;
                    }
                }

                // Generate the edges from start - clip top and clip bottom - end
                // Clip top and clip bottom may be the same vertex if there is only one 
                // clipped vertex.

                // Check that the line is different for the vertices.
                if (mVertexData[startIndex].mLine != clipVertices[topClipIndex].mLine)
                {
                    aEdges[edgeCount].mFirstLine = firstLine;
                    aEdges[edgeCount].mLastLine = clipVertices[topClipIndex].mLine;
                    aEdges[edgeCount].mWinding = winding;

                    // If startIndex is clipped, the edge is a vertical one.
                    if (mVertexData[startIndex].mClipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        aEdges[edgeCount].mX = rationalToFixed(VECTOR2D_GETX(clipVertices[topClipIndex].mPosition));
                        aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                        aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                    }
                    else
                    {
                        RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                            ((RATIONAL)firstLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                        aEdges[edgeCount].mX = rationalToFixed(startx);
                        aEdges[edgeCount].mSlope = rationalToFixed(slope);

                        if (aEdges[edgeCount].mLastLine - aEdges[edgeCount].mFirstLine >= SLOPE_FIX_STEP)
                        {
                            aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                        }
                    }


#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif

                    edgeCount++;
                }

                // Check that the line is different for the vertices.
                if (clipVertices[bottomClipIndex].mLine != mVertexData[endIndex].mLine)
                {
                    int firstClipLine = clipVertices[bottomClipIndex].mLine + 1;

                    aEdges[edgeCount].mFirstLine = firstClipLine;
                    aEdges[edgeCount].mLastLine = lastLine;
                    aEdges[edgeCount].mWinding = winding;

                    // If endIndex is clipped, the edge is a vertical one.
                    if (mVertexData[endIndex].mClipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        aEdges[edgeCount].mX = rationalToFixed(VECTOR2D_GETX(clipVertices[bottomClipIndex].mPosition));
                        aEdges[edgeCount].mSlope = INT_TO_FIXED(0);
                        aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                    }
                    else
                    {
                        RATIONAL startx = VECTOR2D_GETX(mVertexData[startIndex].mPosition) +
                            ((RATIONAL)firstClipLine - VECTOR2D_GETY(mVertexData[startIndex].mPosition)) * slope;

                        aEdges[edgeCount].mX = rationalToFixed(startx);
                        aEdges[edgeCount].mSlope = rationalToFixed(slope);

                        if (aEdges[edgeCount].mLastLine - aEdges[edgeCount].mFirstLine >= SLOPE_FIX_STEP)
                        {
                            aEdges[edgeCount].mSlopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                (aEdges[edgeCount].mSlope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            aEdges[edgeCount].mSlopeFix = INT_TO_FIXED(0);
                        }
                    }


#ifdef SUBPOLYGON_DEBUG
                    DEBUG_ASSERT(verifyEdge(aEdges[edgeCount], aClipRectangle));
#endif

                    edgeCount++;
                }

            }
        }
    }

    return edgeCount;
}


//! A class for handling polygons. Each polygon is a set of sub-polygons.
class Polygon : public PolygonWrapper
{
public:
    //! Constructor.
    /*! Note that the ownership of the sub-polygon array is transferred.
     */
    Polygon(SubPolygon** aSubPolygons, int aSubPolygonCount);

    //! Virtual destructor.
    virtual ~Polygon();

    //! Returns the amount of sub-polygons.
    inline int getSubPolygonCount() const
    {
        return mSubPolygonCount;
    }

    //! Returns a sub-polygon at given index.
    inline const SubPolygon& getSubPolygon(int aIndex) const
    {
        return *mSubPolygons[aIndex];
    }

    //! Static method for creating a polygon from the data.
    static Polygon* create(const PolygonData* aPolygonData);

protected:
    int mSubPolygonCount;
    SubPolygon** mSubPolygons;
};
Polygon::Polygon(SubPolygon** aSubPolygons, int aSubPolygonCount)
{
    mSubPolygons = aSubPolygons;
    mSubPolygonCount = aSubPolygonCount;
}




//! Virtual destructor.
Polygon::~Polygon()
{
    int n;
    for (n = 0; n < mSubPolygonCount; n++)
        delete mSubPolygons[n];
    delete[] mSubPolygons;
}


//! Static method for creating a polygon from the data.
Polygon* Polygon::create(const PolygonData* aPolygonData)
{
    bool error = false;

    int subPolygonCount = aPolygonData->getSubPolygonCount();

    Polygon* polygon = NULL;
    SubPolygon** subPolygons = new SubPolygon * [subPolygonCount];

    if (subPolygons == NULL)
        error = true;

    if (!error)
    {
        int n;
        for (n = 0; n < subPolygonCount; n++)
            subPolygons[n] = NULL;

        n = 0;
        while (!error && n < subPolygonCount)
        {
            int vertexCount = aPolygonData->getVertexCount(n);

            VECTOR2D* vertices = new VECTOR2D[vertexCount];
            if (vertices == NULL)
                error = true;

            if (!error)
            {
                int p;
                const RATIONAL* vertexData = aPolygonData->getVertexData(n);
                for (p = 0; p < vertexCount; p++)
                {
                    VECTOR2D_SETX(vertices[p], vertexData[p * 2]);
                    VECTOR2D_SETY(vertices[p], vertexData[p * 2 + 1]);
                }

                subPolygons[n] = new SubPolygon(vertices, vertexCount);
                if (subPolygons[n] == NULL || !subPolygons[n]->init())
                {
                    delete[] vertices;
                    error = true;
                }
            }

            n++;
        }
    }

    if (!error)
    {
        polygon = new Polygon(subPolygons, subPolygonCount);
        if (polygon == NULL)
            error = true;
    }

    if (error)
    {
        int n;
        for (n = 0; n < subPolygonCount; n++)
            delete subPolygons[n];
        delete[] subPolygons;
        return NULL;
    }

    return polygon;
}
class PaintWrapper
{
public:
    //! Virtual destructor.
    virtual ~PaintWrapper() {};
};
//! Implements a PaintWrapper that contains just the paint colour as unsigned int.
class DefaultPaintWrapper : public PaintWrapper
{
public:
    //! Constructor.
    DefaultPaintWrapper(unsigned int aColor)
    {
        mColor = aColor;
    }

    //! Virtual destructor.
    virtual ~DefaultPaintWrapper() {}

    //! Inline getter for the color.
    inline unsigned int getColor()
    {
        return mColor;
    }

protected:
    unsigned int mColor;
};
#define RENDERER_FILLMODE unsigned char
#define RENDERER_FILLMODE_EVENODD 0
#define RENDERER_FILLMODE_NONZERO 1
//! A base class for factories that create the actual polygon implementations.
class PolygonData;
class PolygonFactory
{
public:
    //! Notifies the factory that a construction of new vector object is starting.
    virtual void beginGraphic() {}

    //! Creates a polygon with the factory.
    virtual PolygonWrapper* createPolygonWrapper(const PolygonData* aData) = 0;

    //! Creates a paint with the factory from an RGB color.
    virtual PaintWrapper* createPaintWrapper(const unsigned int aColor)
    {
        return new DefaultPaintWrapper(aColor);
    }

    //! Notifies the factory that a construction of a vector object has ended.
    virtual void endGraphic() {}
};

class RenderTargetWrapper
{
public:
    //! A callback for notifying that the rendering has finished.
    virtual void onRenderDone() {}
};
class BitmapData : public RenderTargetWrapper
{
public:
    enum BitmapFormat
    {
        BITMAP_FORMAT_XRGB = 0,
        BITMAP_FORMAT_ARGB,
        BITMAP_FORMAT_XBGR,
        BITMAP_FORMAT_ABGR,
        BITMAP_FORMAT_RGBX,
        BITMAP_FORMAT_RGBA,
        BITMAP_FORMAT_BGRX,
        BITMAP_FORMAT_BGRA
    };

    //! Constructor.
    /*! /param aWidth the width of the bitmap in pixels.
     *  /param aHeight the height of the bitmap in pixels.
     *  /param aPitch the lenght of one scanline in bytes.
     *  /param aData a pointer to the data.
     *  /param aFormat the format of the bitmap.
     */
    BitmapData(unsigned int aWidth, unsigned int aHeight,
        unsigned int aPitch, unsigned long* aData,
        BitmapFormat aFormat) :
        mWidth(aWidth), mHeight(aHeight), mPitch(aPitch), mData(aData),
        mFormat(aFormat) {}

    unsigned int mWidth;
    unsigned int mHeight;
    unsigned int mPitch;
    unsigned long* mData;
    BitmapFormat mFormat;
};
class SpanExtents
{
public:
    // Marks a span. aStart and aEnd should be sorted.
    inline void mark(int aStart, int aEnd)
    {
        if (aStart < mMinimum)
            mMinimum = aStart;
        if (aEnd > mMaximum)
            mMaximum = aEnd;
    }

    // Marks a span. aStart and aEnd don't have to be sorted.
    inline void markWithSort(int aStart, int aEnd)
    {
        if (aStart <= aEnd)
            mark(aStart, aEnd);
        else
            mark(aEnd, aStart);
    }

    inline void reset()
    {
        mMinimum = 0x7fffffff;
        mMaximum = 0x80000000;
    }

    int mMinimum = 0x7fffffff;
    int mMaximum = 0x80000000;
};
class Renderer
{
public:
    //! Virtual destructor.
    virtual ~Renderer() {}
    virtual void render(RenderTargetWrapper* aTarget, PolygonWrapper* aPolygon, PaintWrapper* aColor,
        RENDERER_FILLMODE aFillMode, const MATRIX2D& aTransformation) = 0;
};
//! Defines the API for the polygon fillers.
class PolygonFiller : public Renderer
{
public:
    //! Virtual destructor.
    virtual ~PolygonFiller() {}

    //! Renders a polygon.
    /*! /param aTarget the render target for rendering the polygon.
     *  /param aPolygon the polygon to render.
     *  /param aColor the rendering color.
     *  /param aFillMode the fill mode for rendering.
     *  /param aTransformation the transformation for rendering.
     */
    virtual void render(RenderTargetWrapper* aTarget, PolygonWrapper* aPolygon, PaintWrapper* aColor,
        RENDERER_FILLMODE aFillMode, const MATRIX2D& aTransformation) {
        unsigned int color = ((DefaultPaintWrapper*)aColor)->getColor();
        if (aFillMode == RENDERER_FILLMODE_EVENODD)
            renderEvenOdd((BitmapData*)aTarget, (Polygon*)aPolygon, color, aTransformation);
        else
            renderNonZeroWinding((BitmapData*)aTarget, (Polygon*)aPolygon, color, aTransformation);
    }

    //! Renders the polygon with even-odd fill.
    /*! \param aTarget the target bitmap.
     *  \param aPolygon the polygon to render.
     *  \param aColor the color to be used for rendering.
     *  \param aTransformation the transformation matrix.
     */
    virtual void renderEvenOdd(BitmapData* aTarget, const Polygon* aPolygon, unsigned long aColor, const MATRIX2D& aTransformation) = 0;

    //! Renders the polygon with non-zero winding fill.
    /*! \param aTarget the target bitmap.
     *  \param aPolygon the polygon to render.
     *  \param aColor the color to be used for rendering.
     *  \param aTransformation the transformation matrix.
     */
    virtual void renderNonZeroWinding(BitmapData* aTarget, const Polygon* aPolygon, unsigned long aColor, const MATRIX2D& aTransformation) = 0;

    //! Sets the clip rectangle for the polygon filler.
    virtual void setClipRect(unsigned int aX, unsigned int aY, unsigned int aWidth, unsigned int aHeight) = 0;

#ifdef GATHER_STATISTICS
    //! Returns the statistics for the filling operation.
    const PolygonRasterizationStatistics& getStatistics()
    {
        return mStatistics;
    }

    //! Resets the statistics counters.
    void resetStatistics()
    {
        mStatistics.reset();
    }
#endif

protected:
#ifdef GATHER_STATISTICS
    PolygonRasterizationStatistics mStatistics;
#endif
};
class PolygonVersionF : public PolygonFiller
{
public:

    //! Constructor.
    PolygonVersionF();

    //! Initializer.
    /*! aWidth and aHeight define the maximum output size for the filler.
     *  The filler will output to larger bitmaps as well, but the output will
     *  be cropped. aEdgeCount defines the initial amount of edges available for
     *  rendering.
     */
    bool init(unsigned int aWidth, unsigned int aHeight, unsigned int aEdgeCount);

    //! Virtual destructor.
    virtual ~PolygonVersionF();

    //! Renders the polygon with even-odd fill.
    /*! \param aTarget the target bitmap.
     *  \param aPolygon the polygon to render.
     *  \param aColor the color to be used for rendering.
     *  \param aTransformation the transformation matrix.
     */
    virtual void renderEvenOdd(BitmapData* aTarget, const Polygon* aPolygon, unsigned long aColor, const MATRIX2D& aTransformation);

    //! Renders the polygon with non-zero winding fill.
    /*! \param aTarget the target bitmap.
     *  \param aPolygon the polygon to render.
     *  \param aColor the color to be used for rendering.
     *  \param aTransformation the transformation matrix.
     */
    virtual void renderNonZeroWinding(BitmapData* aTarget, const Polygon* aPolygon, unsigned long aColor, const MATRIX2D& aTransformation);

    //! Sets the clip rectangle for the polygon filler.
    virtual void setClipRect(unsigned int aX, unsigned int aY, unsigned int aWidth, unsigned int aHeight);

protected:
    //! Checks if a surface format is supported.
    bool isSupported(BitmapData::BitmapFormat aFormat);

    //! Adds an edge.
    bool addEdge(const PolygonEdge& aEdge);

    //! Renders the edges from the current vertical index using even-odd fill.
    inline void renderEvenOddEdges(PolygonScanEdge*& aActiveEdgeTable, SpanExtents& aEdgeExtents, int aCurrentLine);

    //! Renders the mask to the canvas with even-odd fill.
    void fillEvenOdd(BitmapData* aTarget, unsigned long aColor);

    //! Renders the edges from the current vertical index using non-zero winding fill.
    inline void renderNonZeroEdges(PolygonScanEdge*& aActiveEdgeTable, SpanExtents& aEdgeExtents, int aCurrentLine);

    //! Renders the mask to the canvas with non-zero winding fill.
    void fillNonZero(BitmapData* aTarget, unsigned long aColor, const ClipRectangle& aClipRect);

    //! Resets the fill extents.
    void resetExtents();

    //! Resizes the edge storage.
    bool resizeEdgeStorage(int aIncrement);

    //! Returns the amount of free edges in the edge storage.
    inline int getFreeEdgeCount()
    {
        return mEdgeCount - mCurrentEdge - 1;
    }

    SUBPIXEL_DATA* mMaskBuffer;
    NonZeroMask* mWindingBuffer;
    SpanExtents mVerticalExtents;

    PolygonScanEdge** mEdgeTable;
    PolygonScanEdge* mEdgeStorage;
    unsigned int mEdgeCount;
    unsigned int mCurrentEdge;

    unsigned int mWidth;
    unsigned int mBufferWidth;
    unsigned int mHeight;

    ClipRectangle mClipRect;
    MATRIX2D mRemappingMatrix;
};


//! A class for creating the default polygon implementations.
class DefaultPolygonFactory : public PolygonFactory
{
public:

    //! Creates a polygon with the factory.
    virtual PolygonWrapper* createPolygonWrapper(const PolygonData* aData)
    {
        return Polygon::create(aData);
    }

protected:
};

class VectorGraphic
{
public:
    //! Constructor.
    VectorGraphic(PolygonWrapper** aPolygons, PaintWrapper** aColors, RENDERER_FILLMODE* aFillModes, int aPolygonCount);

    //! Destructor.
    ~VectorGraphic();

    //! Returns the polygon count of the graphic.
    inline int getPolygonCount()
    {
        return mPolygonCount;
    }

    //! Returns the polygon at given index.
    inline const PolygonWrapper* getPolygon(int aIndex)
    {
        return mPolygons[aIndex];
    }

    //! Returns the color at given index.
    inline const PaintWrapper* getColor(int aIndex)
    {
        return mColors[aIndex];
    }

    //! Returns the fill mode at given index.
    inline RENDERER_FILLMODE getFillMode(int aIndex)
    {
        return mFillModes[aIndex];
    }

    //! Renders the vector graphic with given renderer.
    /*! /param aRenderer the class to use for rendering.
     *  /param aTarget the render target.
     *  /param aTransformation the transformation to use in the rendering.
     */
    void render(Renderer* aRenderer, RenderTargetWrapper* aTarget, const MATRIX2D& aTransformation);
    //! Static method for creating the vector graphic from data.
    static VectorGraphic* create(const PolygonData* const* aPolygons, const unsigned long* aColors,
        const RENDERER_FILLMODE* aFillModes, int aPolygonCount,
        PolygonFactory* aFactory);


protected:
    int mPolygonCount;
    PolygonWrapper** mPolygons;
    PaintWrapper** mColors;
    RENDERER_FILLMODE* mFillModes;
};


//! Constructor.
VectorGraphic::VectorGraphic(PolygonWrapper** aPolygons, PaintWrapper** aColors, RENDERER_FILLMODE* aFillModes, int aPolygonCount) :
    mPolygons(aPolygons), mColors(aColors), mFillModes(aFillModes), mPolygonCount(aPolygonCount)
{

}


//! Destructor.
VectorGraphic::~VectorGraphic()
{
    int n;
    for (n = 0; n < mPolygonCount; n++)
    {
        delete mPolygons[n];
        delete mColors[n];
    }
    delete[] mPolygons;
    delete[] mColors;
    delete[] mFillModes;
}


//! Renders the vector graphic with given renderer.
/*! /param aRenderer the class to use for rendering.
 *  /param aTarget the render target.
 *  /param aTransformation the transformation to use in the rendering.
 */
void VectorGraphic::render(Renderer* aRenderer, RenderTargetWrapper* aTarget, const MATRIX2D& aTransformation)
{
    int n;
    printf("mPolygonCount %i\n", mPolygonCount);
    for (n = 0; n < mPolygonCount; n++)
    {
        aRenderer->render(aTarget, mPolygons[n], mColors[n], mFillModes[n], aTransformation);
    }

    aTarget->onRenderDone();
}


//! Static method for creating the vector graphic from data.
VectorGraphic* VectorGraphic::create(const PolygonData* const* aPolygons, const unsigned long* aColors,
    const RENDERER_FILLMODE* aFillModes, int aPolygonCount, PolygonFactory* aFactory)
{
    PolygonWrapper** polygons = new PolygonWrapper * [aPolygonCount];
    PaintWrapper** colors = new PaintWrapper * [aPolygonCount];
    RENDERER_FILLMODE* fillModes = new RENDERER_FILLMODE[aPolygonCount];

    if (polygons == NULL || colors == NULL || fillModes == NULL)
    {
        delete[] polygons;
        delete[] colors;
        delete[] fillModes;
        return NULL;
    }

    aFactory->beginGraphic();
    int n;
    for (n = 0; n < aPolygonCount; n++)
    {
        polygons[n] = aFactory->createPolygonWrapper(aPolygons[n]);
        colors[n] = aFactory->createPaintWrapper(aColors[n]);
        if (polygons[n] == NULL || colors[n] == NULL)
        {
            int p;
            for (p = 0; p < n; p++)
            {
                delete polygons[p];
                delete colors[n];
            }
            delete[] polygons;
            delete[] colors;
            delete[] fillModes;
            return NULL;
        }

        fillModes[n] = aFillModes[n];
    }
    aFactory->endGraphic();

    VectorGraphic* vg = new VectorGraphic(polygons, colors, fillModes, aPolygonCount);

    if (vg == NULL)
    {
        for (n = 0; n < aPolygonCount; n++)
        {
            delete polygons[n];
            delete colors[n];
        }
        delete[] polygons;
        delete[] colors;
        delete[] fillModes;
        return NULL;
    }

    return vg;
}

//! Constructor.
PolygonVersionF::PolygonVersionF()
{
    mMaskBuffer = NULL;
    mWindingBuffer = NULL;
    mEdgeTable = NULL;
    mEdgeCount = 0;
    mCurrentEdge = 0;
    // Scale the coordinates by SUBPIXEL_COUNT in vertical direction
    VECTOR2D scale(INT_TO_RATIONAL(1), INT_TO_RATIONAL(SUBPIXEL_COUNT));
    MATRIX2D_MAKESCALING(mRemappingMatrix, scale);
    // The sampling point for the sub-pixel is at the top right corner. This
    // adjustment moves it to the pixel center.
    VECTOR2D translation(FLOAT_TO_RATIONAL(0.5f / SUBPIXEL_COUNT), FLOAT_TO_RATIONAL(-0.5f));
    MATRIX2D_TRANSLATE(mRemappingMatrix, translation);
}


//! Initializer.
/*! aWidth and aHeight define the maximum output size for the filler.
 *  The filler will output to larger bitmaps as well, but the output will
 *  be cropped. aEdgeCount defines the initial amount of edges available for
 *  rendering.
 */
bool PolygonVersionF::init(unsigned int aWidth, unsigned int aHeight, unsigned int aEdgeCount)
{
    // The buffer used for filling needs to be three pixels wider than the bitmap.
    // This is because of three reasons: first, the buffer used for filling needs
    // to be one pixel wider because the end flag that turns the fill off is the
    // first pixel after the actually drawn edge. Second, the edge tracking needs
    // to add the maximum sample offset (i.e. 1) to the end of the edge. This
    // requires one pixel more in the buffer to avoid reading of data from the
    // left edge. Third, the rendering is stopped with an edge marker that is placed
    // after the data, thus requiring one pixel more for the maximum case.
    unsigned int bufferWidth = aWidth + 3;

    mMaskBuffer = new SUBPIXEL_DATA[bufferWidth];
    if (mMaskBuffer == NULL)
        return false;
    MEMSET(mMaskBuffer, 0, bufferWidth * sizeof(SUBPIXEL_DATA));

    mWindingBuffer = new NonZeroMask[bufferWidth];
    if (mWindingBuffer == NULL)
        return false;
    MEMSET(mWindingBuffer, 0, bufferWidth * sizeof(NonZeroMask));

    mEdgeTable = new PolygonScanEdge * [aHeight];
    if (mEdgeTable == NULL)
        return false;
    MEMSET(mEdgeTable, 0, aHeight * sizeof(PolygonScanEdge*));

    mEdgeStorage = new PolygonScanEdge[aEdgeCount];
    if (mEdgeStorage == NULL)
        return false;
    mEdgeCount = aEdgeCount;

    mWidth = aWidth;
    mBufferWidth = bufferWidth;
    mHeight = aHeight;

    mClipRect.setClip(0, 0, mWidth, mHeight, SUBPIXEL_COUNT);

    return true;
}


//! Virtual destructor.
PolygonVersionF::~PolygonVersionF()
{
    delete[] mMaskBuffer;
    delete[] mWindingBuffer;
    delete[] mEdgeTable;
    delete[] mEdgeStorage;
}


//! Renders the polygon with even-odd fill.
/*! \param aTarget the target bitmap.
 *  \param aPolygon the polygon to render.
 *  \param aColor the color to be used for rendering.
 *  \param aTransformation the transformation matrix.
 */
void PolygonVersionF::renderEvenOdd(BitmapData* aTarget, const Polygon* aPolygon, unsigned long aColor, const MATRIX2D& aTransformation)
{
    printf("ODD\n");
    if (!isSupported(aTarget->mFormat))
        return;

    // Sets the round down mode in case it has been modified.
    setRoundDownMode();

    mVerticalExtents.reset();
    mCurrentEdge = 0;

    MATRIX2D transform = aTransformation;
    MATRIX2D_MULTIPLY(transform, mRemappingMatrix);

    ClipRectangle clipRect;
    clipRect.setClip(0, 0, aTarget->mWidth, aTarget->mHeight, SUBPIXEL_COUNT);
    clipRect.intersect(mClipRect);

    bool success = true;

    int subPolyCount = aPolygon->getSubPolygonCount();
    printf("subPolyCount %i\n", subPolyCount);
    int n = 0;
    while (n < subPolyCount && success)
    {
        const SubPolygon& poly = aPolygon->getSubPolygon(n);

        int count = poly.getVertexCount();
        // The maximum amount of edges is 3 x the vertices.
        int freeCount = getFreeEdgeCount() - count * 3;
        if (freeCount < 0 &&
            !resizeEdgeStorage(-freeCount))
        {
            success = false;
        }
        else
        {
            PolygonScanEdge* edges = &mEdgeStorage[mCurrentEdge];
            int edgeCount = poly.getScanEdges(edges, transform, clipRect);

            int p;
            for (p = 0; p < edgeCount; p++)
            {
                int firstLine = edges[p].mFirstLine >> SUBPIXEL_SHIFT;
                int lastLine = edges[p].mLastLine >> SUBPIXEL_SHIFT;

                edges[p].mNextEdge = mEdgeTable[firstLine];
                mEdgeTable[firstLine] = &edges[p];

                mVerticalExtents.mark(firstLine, lastLine);
            }
            mCurrentEdge += edgeCount;
        }

        /*
        // This is the old implementation that uses getEdges function.
        int count = poly.getVertexCount();
        int p = 0;
        while (p < count && success)
        {
            PolygonEdge edges[20];
            int edgeCount = poly.getEdges(p,10,edges,transform,clipRect);
            int k = 0;
            while (k < edgeCount && success)
            {
                success = addEdge(edges[k]);
                k++;
            }
            p += 10;
        }
        */

        n++;
    }

    // success = false;

    if (success)
        fillEvenOdd(aTarget, aColor);
    else
    {
        unsigned int y;
        for (y = 0; y < mHeight; y++)
            mEdgeTable[y] = NULL;
    }
}


//! Renders the polygon with non-zero winding fill.
/*! \param aTarget the target bitmap.
 *  \param aPolygon the polygon to render.
 *  \param aColor the color to be used for rendering.
 *  \param aTransformation the transformation matrix.
 */
void PolygonVersionF::renderNonZeroWinding(BitmapData* aTarget, const Polygon* aPolygon, unsigned long aColor, const MATRIX2D& aTransformation)
{
    printf("renderNonZeroWinding\n");

    if (!isSupported(aTarget->mFormat))
        return;

    // Sets the round down mode in case it has been modified.
    setRoundDownMode();

    mVerticalExtents.reset();
    mCurrentEdge = 0;

    MATRIX2D transform = aTransformation;
    MATRIX2D_MULTIPLY(transform, mRemappingMatrix);

    ClipRectangle clipRect;
    clipRect.setClip(0, 0, aTarget->mWidth, aTarget->mHeight, SUBPIXEL_COUNT);
    clipRect.intersect(mClipRect);

    bool success = true;

    int subPolyCount = aPolygon->getSubPolygonCount();
    int n = 0;
    while (n < subPolyCount && success)
    {
        const SubPolygon& poly = aPolygon->getSubPolygon(n);

        int count = poly.getVertexCount();
        // The maximum amount of edges is 3 x the vertices.
        int freeCount = getFreeEdgeCount() - count * 3;
        if (freeCount < 0 &&
            !resizeEdgeStorage(-freeCount))
        {
            success = false;
        }
        else
        {
            PolygonScanEdge* edges = &mEdgeStorage[mCurrentEdge];
            int edgeCount = poly.getScanEdges(edges, transform, clipRect);

            int p;
            for (p = 0; p < edgeCount; p++)
            {
                int firstLine = edges[p].mFirstLine >> SUBPIXEL_SHIFT;
                int lastLine = edges[p].mLastLine >> SUBPIXEL_SHIFT;

                edges[p].mNextEdge = mEdgeTable[firstLine];
                mEdgeTable[firstLine] = &edges[p];

                mVerticalExtents.mark(firstLine, lastLine);
            }
            mCurrentEdge += edgeCount;
        }

        /*
        // This is the old implementation that uses getEdges function.
        int count = poly.getVertexCount();
        int p = 0;
        while (p < count && success)
        {
            PolygonEdge edges[20];
            int edgeCount = poly.getEdges(p,10,edges,transform,clipRect);
            int k = 0;
            while (k < edgeCount && success)
            {
                success = addEdge(edges[k]);
                k++;
            }
            p += 10;
        }
        */
        n++;
    }

    //    success = false;

    if (success)
        fillNonZero(aTarget, aColor, clipRect);
    else
    {
        unsigned int y;
        for (y = 0; y < mHeight; y++)
            mEdgeTable[y] = NULL;
    }
}


//! Sets the clip rectangle for the polygon filler.
void PolygonVersionF::setClipRect(unsigned int aX, unsigned int aY, unsigned int aWidth, unsigned int aHeight)
{
    unsigned int x = aX;
    unsigned int y = aY;

    if (x > mWidth)
        x = mWidth;

    if (y > mHeight)
        y = mHeight;

    unsigned int maxX = x + aWidth;
    unsigned int maxY = y + aHeight;

    if (maxX > mWidth)
        maxX = mWidth;

    if (maxY > mHeight)
        maxY = mHeight;

    mClipRect.setClip(x, y, maxX - x, maxY - y, SUBPIXEL_COUNT);
}


//! Adds an edge.
bool PolygonVersionF::addEdge(const PolygonEdge& aEdge)
{
    /* Note that this is unused code. Current implementation uses
       SubPolygon::getScanEdges(). */

       // Enough edges in the storage?
    if (mCurrentEdge >= mEdgeCount)
    {
        // Resize if not, exit if resize fails.
        // The resizing rule is 1.5 * size + 1
        if (!resizeEdgeStorage(mEdgeCount / 2 + 1))
            return false;
    }

    PolygonScanEdge* edge = &mEdgeStorage[mCurrentEdge++];

    edge->mFirstLine = aEdge.mFirstLine;
    edge->mLastLine = aEdge.mLastLine;
    edge->mWinding = aEdge.mWinding;
    edge->mX = rationalToFixed(aEdge.mX);
    edge->mSlope = rationalToFixed(aEdge.mSlope);

    // Calculate the index for first and last line.
    int firstLine = aEdge.mFirstLine >> SUBPIXEL_SHIFT;
    int lastLine = aEdge.mLastLine >> SUBPIXEL_SHIFT;

    mVerticalExtents.mark(firstLine, lastLine);

    // Add to the edge table.
    edge->mNextEdge = mEdgeTable[firstLine];
    mEdgeTable[firstLine] = edge;

    return true;
}


//! Renders the edges from the current vertical index using even-odd fill.
inline void PolygonVersionF::renderEvenOddEdges(PolygonScanEdge*& aActiveEdgeTable, SpanExtents& aEdgeExtents, int aCurrentLine)
{
    static const FIXED_POINT offsets[SUBPIXEL_COUNT] = SUBPIXEL_OFFSETS_FIXED;

    // First, process edges in the active edge table. These are either full height edges
    // or edges that end before the last line. Remove those that end within the scan line.

    // Then, fetch the edges from the edge table. These are either such that the edge is
    // fully within this scanline or the edge starts from within the scanline and continues
    // to the next scanline. Add those that continue to the next scanline to the active
    // edge table. Clear the edge table.

    PolygonScanEdge* prevEdge = NULL;
    PolygonScanEdge* currentEdge = aActiveEdgeTable;

    while (currentEdge)
    {
        // The plotting of all edges in the AET starts from the top of the scanline.
        // The plotting is divided to two stages: those that end on this scanline and
        // those that span over the full scanline.

        int lastLine = currentEdge->mLastLine >> SUBPIXEL_SHIFT;

        if (lastLine == aCurrentLine)
        {
            FIXED_POINT x = currentEdge->mX;
            FIXED_POINT slope = currentEdge->mSlope;

            // First x value rounded down.
            int xs = FIXED_TO_INT(x);
            int ye = currentEdge->mLastLine & (SUBPIXEL_COUNT - 1);

            SUBPIXEL_DATA mask = 1;
            int ySub;
            for (ySub = 0; ySub <= ye; ySub++)
            {
                int xp = FIXED_TO_INT(x + offsets[ySub]);
                mMaskBuffer[xp] ^= mask;
                mask <<= 1;
                x += slope;
            }

            // Last x-value rounded down.
            int xe = FIXED_TO_INT(x - slope);

            // Mark the span.
            aEdgeExtents.markWithSort(xs, xe);

            // Remove the edge from the active edge table
            currentEdge = currentEdge->mNextEdge;
            if (prevEdge)
                prevEdge->mNextEdge = currentEdge;
            else
                aActiveEdgeTable = currentEdge;
        }
        else
        {
            FIXED_POINT x = currentEdge->mX;
            FIXED_POINT slope = currentEdge->mSlope;

            // First x value rounded down.
            int xs = FIXED_TO_INT(x);
            int xe;

            EVENODD_LINE_UNROLL_INIT();

            EVENODD_LINE_UNROLL_0(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_1(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_2(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_3(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_4(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_5(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_6(mMaskBuffer, x, slope, offsets);
#if SUBPIXEL_COUNT == 8
            xe = FIXED_TO_INT(x);
#endif
            EVENODD_LINE_UNROLL_7(mMaskBuffer, x, slope, offsets);

#if SUBPIXEL_COUNT > 8
            EVENODD_LINE_UNROLL_8(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_9(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_10(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_11(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_12(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_13(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_14(mMaskBuffer, x, slope, offsets);
#if SUBPIXEL_COUNT == 16
            xe = FIXED_TO_INT(x);
#endif
            EVENODD_LINE_UNROLL_15(mMaskBuffer, x, slope, offsets);
#endif

#if SUBPIXEL_COUNT > 16
            EVENODD_LINE_UNROLL_16(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_17(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_18(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_19(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_20(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_21(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_22(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_23(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_24(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_25(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_26(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_27(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_28(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_29(mMaskBuffer, x, slope, offsets);
            EVENODD_LINE_UNROLL_30(mMaskBuffer, x, slope, offsets);
            xe = FIXED_TO_INT(x);
            EVENODD_LINE_UNROLL_31(mMaskBuffer, x, slope, offsets);
#endif

            // Mark the span.
            aEdgeExtents.markWithSort(xs, xe);

            // Update the edge
            if ((aCurrentLine & SLOPE_FIX_SCANLINE_MASK) == 0)
                currentEdge->mX = x + currentEdge->mSlopeFix;
            else
                currentEdge->mX = x;

            // Proceed forward in the AET.
            prevEdge = currentEdge;
            currentEdge = currentEdge->mNextEdge;
        }
    }

    // Fetch edges from the edge table.
    currentEdge = mEdgeTable[aCurrentLine];

    if (currentEdge)
    {
        // Clear the edge table for this line.
        mEdgeTable[aCurrentLine] = NULL;

        do
        {
            // The plotting of all edges in the edge table starts somewhere from the middle
            // of the scanline, and ends either at the end or at the last scanline. This leads
            // to two cases: either the scanning starts and stops within this scanline, or
            // it continues to the next as well.

            int lastLine = currentEdge->mLastLine >> SUBPIXEL_SHIFT;

            if (lastLine == aCurrentLine)
            {
                FIXED_POINT x = currentEdge->mX;
                FIXED_POINT slope = currentEdge->mSlope;

                // First x value rounded down.
                int xs = FIXED_TO_INT(x);

                int ys = currentEdge->mFirstLine & (SUBPIXEL_COUNT - 1);
                int ye = currentEdge->mLastLine & (SUBPIXEL_COUNT - 1);

                SUBPIXEL_DATA mask = 1 << ys;
                int ySub;
                for (ySub = ys; ySub <= ye; ySub++)
                {
                    int xp = FIXED_TO_INT(x + offsets[ySub]);
                    mMaskBuffer[xp] ^= mask;
                    mask <<= 1;
                    x += slope;
                }

                // Last x-value rounded down.
                int xe = FIXED_TO_INT(x - slope);

                // Mark the span.
                aEdgeExtents.markWithSort(xs, xe);

                // Ignore the edge (don't add to AET)
            }
            else
            {
                FIXED_POINT x = currentEdge->mX;
                FIXED_POINT slope = currentEdge->mSlope;

                // First x value rounded down.
                int xs = FIXED_TO_INT(x);

                int ys = currentEdge->mFirstLine & (SUBPIXEL_COUNT - 1);

                SUBPIXEL_DATA mask = 1 << ys;
                int ySub;
                for (ySub = ys; ySub < SUBPIXEL_COUNT; ySub++)
                {
                    int xp = FIXED_TO_INT(x + offsets[ySub]);
                    mMaskBuffer[xp] ^= mask;
                    mask <<= 1;
                    x += slope;
                }

                // Last x-value rounded down.
                int xe = FIXED_TO_INT(x - slope);

                // Mark the span.
                aEdgeExtents.markWithSort(xs, xe);

                // Update the edge
                currentEdge->mX = x;

                // Add the edge to AET
                if (prevEdge)
                    prevEdge->mNextEdge = currentEdge;
                else
                    aActiveEdgeTable = currentEdge;

                prevEdge = currentEdge;
            }

            currentEdge = currentEdge->mNextEdge;
        } while (currentEdge);
    }

    if (prevEdge)
        prevEdge->mNextEdge = NULL;
}


//! Renders the mask to the canvas with even-odd fill.
void PolygonVersionF::fillEvenOdd(BitmapData* aTarget, unsigned long aColor)
{
    printf("fillEvenOdd\n");

    int y;

    int minY = mVerticalExtents.mMinimum;
    int maxY = mVerticalExtents.mMaximum;

    unsigned long cs1 = aColor & 0xff00ff;
    unsigned long cs2 = (aColor >> 8) & 0xff00ff;

    unsigned int pitch = aTarget->mPitch / 4;
    unsigned long* target = &aTarget->mData[minY * pitch];

    PolygonScanEdge* activeEdges = NULL;
    SpanExtents edgeExtents;

    for (y = minY; y <= maxY; y++)
    {
        edgeExtents.reset();

        renderEvenOddEdges(activeEdges, edgeExtents, y);

        int minX = edgeExtents.mMinimum;
        // Offset values are not taken into account when calculating the extents, so add
        // one to the maximum. This makes sure that full spans are included, as offset
        // values are in the range of 0 to 1.
        int maxX = edgeExtents.mMaximum + 1;

        if (minX < maxX)
        {
            unsigned long* tp = &target[minX];
            SUBPIXEL_DATA* mb = &mMaskBuffer[minX];
            SUBPIXEL_DATA* end = &mMaskBuffer[maxX + 1];

            // Place a marker data at the end position.
            // Since marker data is placed after the last entry to be rendered (thus mask is 0),
            // it forces a state change. This drops the control to the outer while loop, which
            // terminates because of the compare.
            *end = SUBPIXEL_FULL_COVERAGE;

            SUBPIXEL_DATA mask = *mb;
            *mb++ = 0;

            while (mb <= end)
            {
                // The straightforward implementation reads in the value, modifies the mask with it,
                // calculates the coverage from the mask and renders the result with it.

                // Here the mask value is fetched in the previous round. Therefore the write operation
                // needs to be done before the new mask value is changed. This also means that the
                // end marker will never be rendered, as the loop terminates before that.
                if (mask == 0)
                {
                    // Mask is empty, scan forward until mask changes.
                    SUBPIXEL_DATA* sb = mb;
                    do
                    {
                        mask = *mb++;
                    } while (mask == 0); // && mb <= end);
                    mb[-1] = 0;
                    tp += mb - sb;
                }
                else if (mask == SUBPIXEL_FULL_COVERAGE)
                {
                    // Mask has full coverage, fill with aColor until mask changes.
                    SUBPIXEL_DATA temp;
                    do
                    {
#ifdef GATHER_STATISTICS
                        if (*tp != STATISTICS_BG_COLOR)
                            mStatistics.mOverdrawPixels++;
                        mStatistics.mFilledPixels++;
#endif
                        * tp++ = aColor;
                        temp = *mb++;
                    } while (temp == 0); // && mb <= end);
                    mb[-1] = 0;
                    mask ^= temp;
                }
                else
                {
                    // Mask is semitransparent.
                    do
                    {
#ifdef GATHER_STATISTICS
                        if (*tp != STATISTICS_BG_COLOR)
                            mStatistics.mOverdrawPixels++;
                        mStatistics.mAntialiasPixels++;
#endif
                        unsigned long alpha = SUBPIXEL_COVERAGE(mask);

                        // alpha is in range of 0 to SUBPIXEL_COUNT
                        unsigned long invAlpha = SUBPIXEL_COUNT - alpha;

                        unsigned long ct1 = (*tp & 0xff00ff) * invAlpha;
                        unsigned long ct2 = ((*tp >> 8) & 0xff00ff) * invAlpha;

                        ct1 = ((ct1 + cs1 * alpha) >> SUBPIXEL_SHIFT) & 0xff00ff;
                        ct2 = ((ct2 + cs2 * alpha) << (8 - SUBPIXEL_SHIFT)) & 0xff00ff00;

                        *tp++ = ct1 + ct2;

                        mask ^= *mb;
                        *mb++ = 0;
                    } while (!(mask == 0 || mask == SUBPIXEL_FULL_COVERAGE)); // && mb <= end);
                }
            }
        }

        target += pitch;
    }
}


//! Renders the edges from the current vertical index using non-zero winding fill.
inline void PolygonVersionF::renderNonZeroEdges(PolygonScanEdge*& aActiveEdgeTable, SpanExtents& aEdgeExtents, int aCurrentLine)
{
    static const FIXED_POINT offsets[SUBPIXEL_COUNT] = SUBPIXEL_OFFSETS_FIXED;

    // First, process edges in the active edge table. These are either full height edges
    // or edges that end before the last line. Remove those that end within the scan line.

    // Then, fetch the edges from the edge table. These are either such that the edge is
    // fully within this scanline or the edge starts from within the scanline and continues
    // to the next scanline. Add those that continue to the next scanline to the active
    // edge table. Clear the edge table.

    PolygonScanEdge* prevEdge = NULL;
    PolygonScanEdge* currentEdge = aActiveEdgeTable;

    while (currentEdge)
    {
        // The plotting of all edges in the AET starts from the top of the scanline.
        // The plotting is divided to two stages: those that end on this scanline and
        // those that span over the full scanline.

        int lastLine = currentEdge->mLastLine >> SUBPIXEL_SHIFT;

        if (lastLine == aCurrentLine)
        {
            FIXED_POINT x = currentEdge->mX;
            FIXED_POINT slope = currentEdge->mSlope;
            NON_ZERO_MASK_DATA_UNIT winding = (NON_ZERO_MASK_DATA_UNIT)currentEdge->mWinding;

            // First x value rounded down.
            int xs = FIXED_TO_INT(x);
            int ye = currentEdge->mLastLine & (SUBPIXEL_COUNT - 1);

            SUBPIXEL_DATA mask = 1;
            int ySub;
            for (ySub = 0; ySub <= ye; ySub++)
            {
                int xp = FIXED_TO_INT(x + offsets[ySub]);
                mWindingBuffer[xp].mMask |= mask;
                mWindingBuffer[xp].mBuffer[ySub] += winding;
                mask <<= 1;
                x += slope;
            }

            // Last x-value rounded down.
            int xe = FIXED_TO_INT(x - slope);

            // Mark the span.
            aEdgeExtents.markWithSort(xs, xe);

            // Remove the edge from the active edge table
            currentEdge = currentEdge->mNextEdge;
            if (prevEdge)
                prevEdge->mNextEdge = currentEdge;
            else
                aActiveEdgeTable = currentEdge;
        }
        else
        {
            FIXED_POINT x = currentEdge->mX;
            FIXED_POINT slope = currentEdge->mSlope;
            NON_ZERO_MASK_DATA_UNIT winding = (NON_ZERO_MASK_DATA_UNIT)currentEdge->mWinding;

            // First x value rounded down.
            int xs = FIXED_TO_INT(x);
            int xe;

            NONZERO_LINE_UNROLL_INIT();

            NONZERO_LINE_UNROLL_0(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_1(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_2(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_3(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_4(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_5(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_6(mWindingBuffer, x, slope, offsets, winding);
#if SUBPIXEL_COUNT == 8
            xe = FIXED_TO_INT(x);
#endif
            NONZERO_LINE_UNROLL_7(mWindingBuffer, x, slope, offsets, winding);

#if SUBPIXEL_COUNT > 8
            NONZERO_LINE_UNROLL_8(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_9(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_10(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_11(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_12(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_13(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_14(mWindingBuffer, x, slope, offsets, winding);
#if SUBPIXEL_COUNT == 16
            xe = FIXED_TO_INT(x);
#endif
            NONZERO_LINE_UNROLL_15(mWindingBuffer, x, slope, offsets, winding);
#endif

#if SUBPIXEL_COUNT > 16
            NONZERO_LINE_UNROLL_16(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_17(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_18(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_19(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_20(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_21(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_22(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_23(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_24(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_25(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_26(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_27(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_28(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_29(mWindingBuffer, x, slope, offsets, winding);
            NONZERO_LINE_UNROLL_30(mWindingBuffer, x, slope, offsets, winding);
            xe = FIXED_TO_INT(x);
            NONZERO_LINE_UNROLL_31(mWindingBuffer, x, slope, offsets, winding);
#endif
            // Mark the span.
            aEdgeExtents.markWithSort(xs, xe);

            // Update the edge
            if ((aCurrentLine & SLOPE_FIX_SCANLINE_MASK) == 0)
                currentEdge->mX = x + currentEdge->mSlopeFix;
            else
                currentEdge->mX = x;

            // Proceed forward in the AET.
            prevEdge = currentEdge;
            currentEdge = currentEdge->mNextEdge;
        }
    }

    // Fetch edges from the edge table.
    currentEdge = mEdgeTable[aCurrentLine];

    if (currentEdge)
    {
        // Clear the edge table for this line.
        mEdgeTable[aCurrentLine] = NULL;

        do
        {
            // The plotting of all edges in the edge table starts somewhere from the middle
            // of the scanline, and ends either at the end or at the last scanline. This leads
            // to two cases: either the scanning starts and stops within this scanline, or
            // it continues to the next as well.

            int lastLine = currentEdge->mLastLine >> SUBPIXEL_SHIFT;

            if (lastLine == aCurrentLine)
            {
                FIXED_POINT x = currentEdge->mX;
                FIXED_POINT slope = currentEdge->mSlope;
                NON_ZERO_MASK_DATA_UNIT winding = (NON_ZERO_MASK_DATA_UNIT)currentEdge->mWinding;

                // First x value rounded down.
                int xs = FIXED_TO_INT(x);

                int ys = currentEdge->mFirstLine & (SUBPIXEL_COUNT - 1);
                int ye = currentEdge->mLastLine & (SUBPIXEL_COUNT - 1);

                SUBPIXEL_DATA mask = 1 << ys;
                int ySub;
                for (ySub = ys; ySub <= ye; ySub++)
                {
                    int xp = FIXED_TO_INT(x + offsets[ySub]);
                    mWindingBuffer[xp].mMask |= mask;
                    mWindingBuffer[xp].mBuffer[ySub] += winding;
                    mask <<= 1;
                    x += slope;
                }

                // Last x-value rounded down.
                int xe = FIXED_TO_INT(x - slope);

                // Mark the span.
                aEdgeExtents.markWithSort(xs, xe);

                // Ignore the edge (don't add to AET)
            }
            else
            {
                FIXED_POINT x = currentEdge->mX;
                FIXED_POINT slope = currentEdge->mSlope;
                NON_ZERO_MASK_DATA_UNIT winding = (NON_ZERO_MASK_DATA_UNIT)currentEdge->mWinding;

                // First x value rounded down.
                int xs = FIXED_TO_INT(x);

                int ys = currentEdge->mFirstLine & (SUBPIXEL_COUNT - 1);

                SUBPIXEL_DATA mask = 1 << ys;
                int ySub;
                for (ySub = ys; ySub < SUBPIXEL_COUNT; ySub++)
                {
                    int xp = FIXED_TO_INT(x + offsets[ySub]);
                    mWindingBuffer[xp].mMask |= mask;
                    mWindingBuffer[xp].mBuffer[ySub] += winding;
                    mask <<= 1;
                    x += slope;
                }

                // Last x-value rounded down.
                int xe = FIXED_TO_INT(x - slope);

                // Mark the span.
                aEdgeExtents.markWithSort(xs, xe);

                // Update the edge
                currentEdge->mX = x;

                // Add the edge to AET
                if (prevEdge)
                    prevEdge->mNextEdge = currentEdge;
                else
                    aActiveEdgeTable = currentEdge;

                prevEdge = currentEdge;
            }

            currentEdge = currentEdge->mNextEdge;
        } while (currentEdge);

    }

    if (prevEdge)
        prevEdge->mNextEdge = NULL;
}


//! Renders the mask to the canvas with non-zero winding fill.
void PolygonVersionF::fillNonZero(BitmapData* aTarget, unsigned long aColor, const ClipRectangle& aClipRect)
{
    static const FIXED_POINT offsets[SUBPIXEL_COUNT] = SUBPIXEL_OFFSETS_FIXED;
    int y;

    int minY = mVerticalExtents.mMinimum;
    int maxY = mVerticalExtents.mMaximum;

    unsigned long cs1 = aColor & 0xff00ff;
    unsigned long cs2 = (aColor >> 8) & 0xff00ff;

    unsigned int pitch = aTarget->mPitch / 4;
    unsigned long* target = &aTarget->mData[minY * pitch];

    NonZeroMask values;

    PolygonScanEdge* activeEdges = NULL;
    SpanExtents edgeExtents;

    for (y = minY; y <= maxY; y++)
    {
        edgeExtents.reset();

        renderNonZeroEdges(activeEdges, edgeExtents, y);

        int minX = edgeExtents.mMinimum;
        // Offset values are not taken into account when calculating the extents, so add
        // one to the maximum. This makes sure that full spans are included, as offset
        // values are in the range of 0 to 1.
        int maxX = edgeExtents.mMaximum + 1;

        if (minX < maxX)
        {
            unsigned long* tp = &target[minX];
            NonZeroMask* wb = &mWindingBuffer[minX];
            NonZeroMask* end = &mWindingBuffer[maxX + 1];

            // Place a marker data at the end position.
            // Since marker data is placed after the last entry to be rendered (thus mask is 0),
            // it forces a state change. This drops the control to the outer while loop, which
            // terminates because of the compare.
            MEMSET(end, 0xff, sizeof(NonZeroMask));

            NonZeroMask* temp = wb++;
            NonZeroMaskC::reset(*temp, values);

            while (wb <= end)
            {
                // The straightforward implementation reads in the value, modifies the mask with it,
                // calculates the coverage from the mask and renders the result with it.

                // Here the mask value is fetched in the previous round. Therefore the write operation
                // needs to be done before the new mask value is changed. This also means that the
                // end marker will never be rendered, as the loop terminates before that.
                if (values.mMask == 0)
                {
                    do
                    {
                        // Mask is empty, scan forward until mask changes.
                        NonZeroMask* sb = wb;
                        do
                        {
                            temp = wb++;
                        } while (temp->mMask == 0 && wb <= end);

                        int count = wb - sb;
                        tp += count;

                        NonZeroMaskC::init(*temp, values);
                    } while (values.mMask == 0); // && wb <= end);
                }
                else if (values.mMask == SUBPIXEL_FULL_COVERAGE)
                {
                    do
                    {
#ifdef GATHER_STATISTICS
                        if (*tp != STATISTICS_BG_COLOR)
                            mStatistics.mOverdrawPixels++;
                        mStatistics.mFilledPixels++;
#endif

                        // Mask has full coverage, fill with aColor until mask changes.
                        * tp++ = aColor;
                        temp = wb++;

                        if (temp->mMask)
                        {
                            NonZeroMaskC::apply(*temp, values);

                            // A safeguard is needed if the winding value overflows and end marker is not
                            // detected correctly
                            if (wb > end)
                                break;
                        }
                    } while (values.mMask == SUBPIXEL_FULL_COVERAGE); // && wb <= end);
                }
                else
                {
                    // Mask is semitransparent.
                    unsigned long alpha = SUBPIXEL_COVERAGE(values.mMask);

                    // alpha is in range of 0 to SUBPIXEL_COUNT
                    unsigned long invAlpha = SUBPIXEL_COUNT - alpha;
                    unsigned long cs1a = cs1 * alpha;
                    unsigned long cs2a = cs2 * alpha;

                    do
                    {
#ifdef GATHER_STATISTICS
                        if (*tp != STATISTICS_BG_COLOR)
                            mStatistics.mOverdrawPixels++;
                        mStatistics.mAntialiasPixels++;
#endif

                        unsigned long ct1 = (*tp & 0xff00ff) * invAlpha;
                        unsigned long ct2 = ((*tp >> 8) & 0xff00ff) * invAlpha;

                        ct1 = ((ct1 + cs1a) >> SUBPIXEL_SHIFT) & 0xff00ff;
                        ct2 = ((ct2 + cs2a) << (8 - SUBPIXEL_SHIFT)) & 0xff00ff00;

                        *tp++ = ct1 + ct2;

                        temp = wb++;
                        if (temp->mMask)
                        {
                            NonZeroMaskC::apply(*temp, values);

                            alpha = SUBPIXEL_COVERAGE(values.mMask);
                            invAlpha = SUBPIXEL_COUNT - alpha;
                            cs1a = cs1 * alpha;
                            cs2a = cs2 * alpha;

                            // A safeguard is needed if the winding value overflows and end marker is not
                            // detected correctly
                            if (wb > end)
                                break;
                        }
                    } while (values.mMask != 0 && values.mMask != SUBPIXEL_FULL_COVERAGE); // && wb <= end);
                }
            }
        }

        target += pitch;
    }

}


//! Checks if a surface format is supported.
bool PolygonVersionF::isSupported(BitmapData::BitmapFormat aFormat)
{
    // Only non-alpha target formats are supported.
    if (aFormat == BitmapData::BITMAP_FORMAT_XRGB ||
        aFormat == BitmapData::BITMAP_FORMAT_XBGR ||
        aFormat == BitmapData::BITMAP_FORMAT_RGBX ||
        aFormat == BitmapData::BITMAP_FORMAT_BGRX)
    {
        return true;
    }
    else
    {
        return false;
    }
}


//! Resizes the edge storage.
bool PolygonVersionF::resizeEdgeStorage(int aIncrement)
{
    unsigned int newCount = mEdgeCount + aIncrement;

    PolygonScanEdge* newStorage = new PolygonScanEdge[newCount];
    if (newStorage == NULL)
        return false;

    // Some pointer arithmetic to keep the linked lists in sync.
    // Make this 64-bit just for future compatibilty
    INTEGER64 diff = (INTEGER64)newStorage - (INTEGER64)mEdgeStorage;

    unsigned int n;
    for (n = 0; n < mHeight; n++)
    {
        if (mEdgeTable[n] != NULL)
            mEdgeTable[n] = (PolygonScanEdge*)((INTEGER64)mEdgeTable[n] + diff);
    }

    // Copy edge storage...
    MEMCPY(newStorage, mEdgeStorage, sizeof(PolygonScanEdge) * mEdgeCount);

    // ...and fix the pointers.
    for (n = 0; n < mEdgeCount; n++)
    {
        PolygonScanEdge* edge = &newStorage[n];
        if (edge->mNextEdge != NULL)
            edge->mNextEdge = (PolygonScanEdge*)((INTEGER64)edge->mNextEdge + diff);
    }

    delete[] mEdgeStorage;
    mEdgeStorage = newStorage;
    mEdgeCount = newCount;

    return true;
}


class bqFrameworkCallbackCB : public bqFrameworkCallback
{
public:
    bqFrameworkCallbackCB() {}
    virtual ~bqFrameworkCallbackCB() {}
    virtual void OnMessage() {}
};

bool g_run = true;

class bqWindowCallbackCB : public bqWindowCallback
{
public:
    bqWindowCallbackCB() {}
    virtual ~bqWindowCallbackCB() {}

    virtual void OnClose(bqWindow*)
    {
        g_run = false;
    }

    virtual void OnSize(bqWindow* w) 
    {
        bqGS* gs = (bqGS*)GetUserData();
        if(gs)
            gs->OnWindowSize();
    }
};


#include <Windows.h>

#ifdef BQ_WITH_GUI
class MyGUIDrawTextCallback : public bqGUIDrawTextCallback
{
    bqColor m_colorBlack;
    bqGUIFont* m_defaultFont = 0;
public:
    MyGUIDrawTextCallback() {}
    virtual ~MyGUIDrawTextCallback() {}

    virtual bqGUIFont* OnFont(uint32_t, char32_t)
    {
        return m_defaultFont;
    }

    virtual bqColor* OnColor(uint32_t, char32_t)
    {
        return &m_colorBlack;
    }

    void SetFont(bqGUIFont* f)
    {
        m_defaultFont = f;
    }
};

class MyButton : public bqGUIButton
{
public:
    MyButton(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUIButton(w, position, size)
    {
    }

    virtual ~MyButton()
    {
    }

    BQ_PLACEMENT_ALLOCATOR(MyButton);
  
    virtual void OnClickLMB() override
    {
        bqLog::Print("BTN LMB\n");
        SetText(U"BTN");
    }
};

class MyCheckBox : public bqGUICheckRadioBox
{
public:
    MyCheckBox(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUICheckRadioBox(w, position, size)
    {
    }

    virtual ~MyCheckBox() {}
};

class MyRadiobutton : public bqGUICheckRadioBox
{
public:
    MyRadiobutton(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUICheckRadioBox(w, position, size)
    {
        m_asRadioButton = true;
        m_radiouGroup = 1;
    }

    virtual ~MyRadiobutton() {}
};

class MyTextEditor : public bqGUITextEditor
{
public:
    MyTextEditor(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size):
        bqGUITextEditor(w, position, size)
    {}
    virtual ~MyTextEditor() {}
};

class MyListBox : public bqGUIListBox
{
public:
    MyListBox(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size) :
        bqGUIListBox(w, position, size)
    {}
    virtual ~MyListBox() {}
};

class MySlider : public bqGUISlider
{
public:
    MySlider(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size) :
        bqGUISlider(w, position, size)
    {}
    virtual ~MySlider() {}
};

class TestGUIScrollbar : public bqGUIScrollbar
{
public:
    TestGUIScrollbar(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUIScrollbar(w, position, size)
    {}
    virtual ~TestGUIScrollbar() {}
    BQ_PLACEMENT_ALLOCATOR(TestGUIScrollbar);
};
#endif

#include "badcoiq/containers/bqFixedFIFO.h"


class TestPattern
{
public:
    //! Creates a siemens star test pattern.
    static VectorGraphic* siemensStar(RATIONAL aSize, int aSegments, RATIONAL aRotation, const VECTOR2D& aCenter,
        RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
        const char* aSVGDumpName, const char* aBinaryDumpName);

    //! Creates a star test pattern with a fill in the middle.
    static VectorGraphic* filledStar(RATIONAL aSize, int aSegments, RATIONAL aFillSize, RATIONAL aRotation, const VECTOR2D& aCenter,
        RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
        const char* aSVGDumpName, const char* aBinaryDumpName);

    //! Creates a test pattern of concentric circles.
    /*! Note that the winding direction for all circles is the same, so non-zero winding fill rule
     *  should produce just one large disk.
     */
    static VectorGraphic* circles(RATIONAL aSize, RATIONAL aWidth, int aSegments, int aCount, const VECTOR2D& aCenter,
        RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
        const char* aSVGDumpName, const char* aBinaryDumpName);

    //! Creates a test pattern of random polygon.
    static VectorGraphic* randomPolygon(RATIONAL aSize, int aVertexCount, int aSeed, RATIONAL aMinStep, RATIONAL aMaxStep, const VECTOR2D& aCenter,
        RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
        const char* aSVGDumpName, const char* aBinaryDumpName);


    //! Creates a test pattern of a star.
    static VectorGraphic* star(RATIONAL aSize, int aSpikeCount, const VECTOR2D& aCenter,
        RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
        const char* aSVGDumpName, const char* aBinaryDumpName);

    //! Creates a test pattern of a checker board.
    static VectorGraphic* checkers(RATIONAL aSize, int aCount, const VECTOR2D& aCenter,
        RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
        const char* aSVGDumpName, const char* aBinaryDumpName);

    //! Creates a test pattern of a set of squares.
    static VectorGraphic* squares(RATIONAL aSquareSize, RATIONAL aGridCellSize, int aGridSteps,
        int aSubPolygonCount, RATIONAL aSizeDelta, int aSegmentCount,
        const VECTOR2D& aCenter, RENDERER_FILLMODE aFillMode, unsigned long aColor,
        unsigned long aColorDelta, PolygonFactory* aFactory, const char* aSVGDumpName,
        const char* aBinaryDumpName);

    //! Creates a test polygon with Zack Rusin's QT test polygons (from http://ktown.kde.org/~zrusin/complex.data)
    /*! aIndex should be in the range of 0 to 2.
     */
    static VectorGraphic* qtPolygon(int aIndex,
        RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
        const char* aSVGDumpName, const char* aBinaryDumpName);


protected:
    //! Creates a set of vertices for a star.
    static RATIONAL* starVertices(RATIONAL aSize, int aCount, const VECTOR2D& aCenter, RATIONAL aStartAngle, RATIONAL aAngleDelta);

};

VectorGraphic* TestPattern::circles(RATIONAL aSize, RATIONAL aWidth, int aSegments, int aCount, const VECTOR2D& aCenter,
    RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
    const char* aSVGDumpName, const char* aBinaryDumpName)
{
    if (aCount < 1)
        return NULL;

    if (aSegments < 3)
        return NULL;

    bool error = false;

    int count = aCount * 2;
    int n;
    int* vertexCounts = new int[count];
    if (vertexCounts)
    {
        for (n = 0; n < count; n++)
            vertexCounts[n] = aSegments;
    }

    RATIONAL** vertexData = new RATIONAL * [count];

    if (vertexData)
    {
        for (n = 0; n < count; n++)
            vertexData[n] = new RATIONAL[2 * aSegments];
    }

    if (vertexData == NULL || vertexCounts == NULL)
        error = true;

    if (!error)
    {
        for (n = 0; n < count; n++)
        {
            if (vertexData[n] == NULL)
                error = true;
        }
    }

    VectorGraphic* vg = NULL;

    if (!error)
    {
        RATIONAL step = FLOAT_TO_RATIONAL(2 * 3.1415926535897932384626433832795f) / aSegments;
        RATIONAL angle = INT_TO_RATIONAL(0);

        // For every line, make two circles.
        for (n = 0; n < aCount; n++)
        {
            RATIONAL outerSize = (RATIONAL)(n + 1) * aSize / (RATIONAL)aCount;
            RATIONAL innerSize = outerSize - aWidth;

            int current = 0;
            RATIONAL* innerVertices = vertexData[n * 2];
            RATIONAL* outerVertices = vertexData[n * 2 + 1];
            int p;
            for (p = 0; p < aSegments; p++)
            {
                RATIONAL sinVal = RATIONAL_SIN(angle);
                RATIONAL cosVal = RATIONAL_COS(angle);

                innerVertices[current] = VECTOR2D_GETX(aCenter) + sinVal * innerSize;
                outerVertices[current++] = VECTOR2D_GETX(aCenter) + sinVal * outerSize;

                innerVertices[current] = VECTOR2D_GETY(aCenter) + cosVal * innerSize;
                outerVertices[current++] = VECTOR2D_GETY(aCenter) + cosVal * outerSize;

                angle += step;
            }
        }

        PolygonData pdata(vertexData, vertexCounts, count);
        PolygonData* polygons[1] = { &pdata };

        unsigned long colors[1] = { aColor };
        RENDERER_FILLMODE fillmodes[1] = { aFillMode };

        vg = VectorGraphic::create(polygons, colors, fillmodes, 1, aFactory);
    }

    if (vertexData)
    {
        for (n = 0; n < count; n++)
            delete[] vertexData[n];
        delete[] vertexData;
    }

    delete vertexCounts;

    return vg;
}

VectorGraphic* TestPattern::randomPolygon(RATIONAL aSize, int aVertexCount, int aSeed, RATIONAL aMinStep, RATIONAL aMaxStep, const VECTOR2D& aCenter,
    RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
    const char* aSVGDumpName, const char* aBinaryDumpName)
{
    if (aVertexCount < 3)
        return NULL;

    RATIONAL* vertices = new RATIONAL[2 * aVertexCount];
    if (!vertices)
        return NULL;

    int rnd = aSeed;

#define RAND_MULTIPLIER 1103515245
#define RAND_ADDER 12345
#define UPDATE_RAND(a) RAND_MULTIPLIER * a + RAND_ADDER

    rnd = UPDATE_RAND(rnd);
    int xi = rnd >> 16;
    rnd = UPDATE_RAND(rnd);
    int yi = rnd >> 16;

    RATIONAL x = (RATIONAL)xi * aSize / INT_TO_RATIONAL(0x7fff);
    RATIONAL y = (RATIONAL)yi * aSize / INT_TO_RATIONAL(0x7fff);

    RATIONAL minSq = aMinStep * aMinStep;
    RATIONAL maxSq = aMaxStep * aMaxStep;
    RATIONAL sizeSq = aSize * aSize;

    int current = 0;
    int n;
    for (n = 0; n < aVertexCount; n++)
    {
        RATIONAL xs, xt, ys, yt, lsq;
        do
        {
            do
            {
                rnd = UPDATE_RAND(rnd);
                xi = rnd >> 16;
                rnd = UPDATE_RAND(rnd);
                yi = rnd >> 16;

                xs = (RATIONAL)xi * aMaxStep / INT_TO_RATIONAL(0x7fff);
                ys = (RATIONAL)yi * aMaxStep / INT_TO_RATIONAL(0x7fff);

                lsq = xs * xs + ys * ys;
            } while (lsq > maxSq || lsq < minSq);

            xt = x + xs;
            yt = y + ys;

            lsq = xt * xt + yt * yt;
        } while (lsq > sizeSq);

        x = xt;
        y = yt;

        vertices[current++] = VECTOR2D_GETX(aCenter) + x;
        vertices[current++] = VECTOR2D_GETY(aCenter) + y;
    }

    RATIONAL* vertexData[1] = { vertices };
    int vertexCounts[1] = { aVertexCount };

    PolygonData pdata(vertexData, vertexCounts, 1);
    PolygonData* polygons[1] = { &pdata };

    unsigned long colors[1] = { aColor };
    RENDERER_FILLMODE fillmodes[1] = { aFillMode };
    VectorGraphic* vg = VectorGraphic::create(polygons, colors, fillmodes, 1, aFactory);

    delete[] vertices;

    return vg;
}


//! Creates a set of vertices for a star.
RATIONAL* TestPattern::starVertices(RATIONAL aSize, int aCount, const VECTOR2D& aCenter, RATIONAL aStartAngle, RATIONAL aAngleDelta)
{
    RATIONAL* vertices = new RATIONAL[2 * aCount];
    if (!vertices)
        return NULL;

    RATIONAL angle = aStartAngle;

    int current = 0;
    int n;
    for (n = 0; n < aCount; n++)
    {
        vertices[current++] = VECTOR2D_GETX(aCenter) + aSize * RATIONAL_SIN(angle);
        vertices[current++] = VECTOR2D_GETY(aCenter) + aSize * RATIONAL_COS(angle);

        angle += aAngleDelta;
    }

    return vertices;
}

VectorGraphic* TestPattern::star(RATIONAL aSize, int aSpikeCount, const VECTOR2D& aCenter,
    RENDERER_FILLMODE aFillMode, unsigned long aColor, PolygonFactory* aFactory,
    const char* aSVGDumpName, const char* aBinaryDumpName)
{
    if (aSpikeCount < 5)
        return NULL;

    if (aSpikeCount & 1)
    {
        //RATIONAL angle = FLOAT_TO_RATIONAL(2.0f * 3.1415926535897932384626433832795f) / (RATIONAL)aSpikeCount;
        //RATIONAL startAngle = FLOAT_TO_RATIONAL(1.0f * 3.1415926535897932384626433832795f);
        //RATIONAL angleDelta = FLOAT_TO_RATIONAL(3.1415926535897932384626433832795f) + angle / INT_TO_RATIONAL(2);

       // RATIONAL *vertices = starVertices(aSize,aSpikeCount,aCenter,startAngle,angleDelta);
       // if (vertices == NULL)
       //     return NULL;

        RATIONAL* vertices = new RATIONAL[6];
        if (!vertices)
            return NULL;

        vertices[0] = 0.f;
        vertices[1] = 0.f;

        vertices[2] = 10.f;
        vertices[3] = 0.f;

        vertices[4] = 10.f;
        vertices[5] = 10.f;

        RATIONAL* vertexData[1] = { vertices };
        int vertexCounts[1] = { 3 };

        PolygonData pdata(vertexData, vertexCounts, 1);
        PolygonData* polygons[1] = { &pdata };

        unsigned long colors[1] = { aColor };
        RENDERER_FILLMODE fillmodes[1] = { aFillMode };

        VectorGraphic* vg = VectorGraphic::create(polygons, colors, fillmodes, 1, aFactory);

        delete[] vertices;

        return vg;
    }
    else if (aSpikeCount & 2)
    {
        int count = aSpikeCount / 2;

        RATIONAL angle = FLOAT_TO_RATIONAL(2.0f * 3.1415926535897932384626433832795f) / (RATIONAL)aSpikeCount;
        RATIONAL startAngleA = FLOAT_TO_RATIONAL(0.5f * 3.1415926535897932384626433832795f);
        RATIONAL startAngleB = FLOAT_TO_RATIONAL(-0.5f * 3.1415926535897932384626433832795f);
        RATIONAL angleDelta = FLOAT_TO_RATIONAL(3.1415926535897932384626433832795f) + angle;

        RATIONAL* verticesA = starVertices(aSize, count, aCenter, startAngleA, angleDelta);
        if (verticesA == NULL)
            return NULL;

        RATIONAL* verticesB = starVertices(aSize, count, aCenter, startAngleB, angleDelta);
        if (verticesB == NULL)
        {
            delete[] verticesA;
            return NULL;
        }

        RATIONAL* vertexData[2] = { verticesA, verticesB };
        int vertexCounts[2] = { count, count };

        PolygonData pdata(vertexData, vertexCounts, 2);
        PolygonData* polygons[1] = { &pdata };

        unsigned long colors[1] = { aColor };
        RENDERER_FILLMODE fillmodes[1] = { aFillMode };


        VectorGraphic* vg = VectorGraphic::create(polygons, colors, fillmodes, 1, aFactory);

        delete[] verticesA;
        delete[] verticesB;

        return vg;
    }
    else
    {
        RATIONAL angle = FLOAT_TO_RATIONAL(2.0f * 3.1415926535897932384626433832795f) / (RATIONAL)aSpikeCount;
        RATIONAL startAngle = FLOAT_TO_RATIONAL(0.5f * 3.1415926535897932384626433832795f);
        RATIONAL angleDelta = FLOAT_TO_RATIONAL(3.1415926535897932384626433832795f) + angle;// / INT_TO_RATIONAL(2);

        RATIONAL* vertices = starVertices(aSize, aSpikeCount, aCenter, startAngle, angleDelta);
        if (vertices == NULL)
            return NULL;

        RATIONAL* vertexData[1] = { vertices };
        int vertexCounts[1] = { aSpikeCount };

        PolygonData pdata(vertexData, vertexCounts, 1);
        PolygonData* polygons[1] = { &pdata };

        unsigned long colors[1] = { aColor };
        RENDERER_FILLMODE fillmodes[1] = { aFillMode };

        VectorGraphic* vg = VectorGraphic::create(polygons, colors, fillmodes, 1, aFactory);

        delete[] vertices;

        return vg;
    }
}



int main()
{
    bqFrameworkCallbackCB fcb;
    bqFramework::Start(&fcb);

    bqWindowCallbackCB wcb;
    auto window = bqFramework::SummonWindow(&wcb);
    if (window && bqFramework::GetGSNum())
    {
        window->SetPositionAndSize(10, 10, 800, 600);
        window->SetVisible(true);

        bqGS* gs = bqFramework::SummonGS(bqFramework::GetGSUID(0));
        if (gs)
        {
            wcb.SetUserData(gs);

            if (gs->Init(window, 0))
            {
                gs->SetClearColor(0.3f, 0.3f, 0.3f, 1.f);

                bqCamera camera;
                camera.m_position.Set(8.f, 8.f, 0.0f, 0.f);
                camera.m_aspect = 300.f / 200.f;
                camera.m_fov = 1.1f;
                
                camera.SetType(bqCamera::Type::PerspectiveLookAt);

                camera.m_rotation.Set(bqMath::DegToRad(-75.f), 0.0, 0.f);
                camera.Update(*bqFramework::GetDeltaTime());
                
               // bqMath::LookAtRH(camera.m_view, camera.m_position, bqVec4(), bqVec4(0.f, 1.f, 0.f, 0.f));
                bqMat4 ViewProjection = camera.GetMatrixProjection() * camera.GetMatrixView();
                bqFramework::SetMatrix(bqMatrixType::ViewProjection, &ViewProjection);
                
                bqMaterial m;
                m.m_sunPosition.Set(1.f, 3.f, -1.f);
                m.m_sunPosition.Normalize();
                m.m_shaderType = bqShaderType::Standart;
               // m.m_maps[0].m_texture = texture;
                gs->SetMaterial(&m);

#ifdef BQ_WITH_GUI
                bqFramework::InitDefaultFonts(gs);
                MyGUIDrawTextCallback tdcb;
                tdcb.SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

                auto guiWindow = bqFramework::SummonGUIWindow(window, bqVec2f(100.f, 100.f), bqVec2f(300.f));
                guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_withTitleBar;
                guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_canMove;
                guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_canResize;
           //     guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_disableScrollbar;
                MyButton* btn = new MyButton(guiWindow, bqVec2f(0.f, 0.f), bqVec2f(50.f, 420.f));
                MyCheckBox* chckbx = new MyCheckBox(guiWindow, bqVec2f(60.f, 0.f), bqVec2f(100.f, 20.f));
                chckbx->SetText(U"Use thing");
                MyRadiobutton* rdbtn1 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 20.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn2 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 40.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn3 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 60.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn4 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 80.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn5 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 100.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn6 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 120.f), bqVec2f(100.f, 20.f));
                rdbtn4->m_radiouGroup = 2;
                rdbtn5->m_radiouGroup = 2;
                rdbtn6->m_radiouGroup = 2;
                rdbtn1->SetText(U"Radio1 g1");
                rdbtn2->SetText(U"Radio2 g1");
                rdbtn3->SetText(U"Radio3 g1");
                rdbtn4->SetText(U"Radio4 g2");
                rdbtn5->SetText(U"Radio5 g2");
                rdbtn6->SetText(U"Radio6 g2");
                rdbtn1->m_isChecked = true;
                rdbtn5->m_isChecked = true;
                MyTextEditor* txtedtr = new MyTextEditor(guiWindow, bqVec2f(60.f, 140.f), bqVec2f(150.f, 200.f));
                MyListBox* lstbx = new MyListBox(guiWindow, bqVec2f(180.f, 20.f), bqVec2f(60.f, 70.f));
                lstbx->AddItem(U"Item1", 0, 0);
                lstbx->AddItem(U"Item12", 0, 0);
                lstbx->AddItem(U"Item13", 0, 0);
                lstbx->AddItem(U"Item14", 0, 0);
                lstbx->AddItem(U"Item15", 0, 0);
                lstbx->AddItem(U"Item16", 0, 0);
                lstbx->AddItem(U"Item17", 0, 0);
                lstbx->AddItem(U"Item18", 0, 0);
                lstbx->AddItem(U"Item19", 0, 0);
                lstbx->AddItem(U"Item10", 0, 0);
                lstbx->AddItem(U"Itemq1", 0, 0);
                lstbx->AddItem(U"Itemw1", 0, 0);
                lstbx->AddItem(U"Iteme1", 0, 0);
                lstbx->AddItem(U"Item1tr", 0, 0);
                lstbx->AddItem(U"Item1y", 0, 0);
                lstbx->AddItem(U"Item1i", 0, 0);
                lstbx->AddItem(U"Item1o", 0, 0);
                lstbx->AddItem(U"Item1p", 0, 0);
                lstbx->AddItem(U"Item1k", 0, 0);
                MySlider* sld = new MySlider(guiWindow, bqVec2f(180.f, 100.f), bqVec2f(90.f, 30.f));
                sld->m_morePrecise = true;
              /*  TestGUIScrollbar* testScrollbar = new TestGUIScrollbar(guiWindow, bqVec2f(), bqVec2f(10.f, 40.f));
                testScrollbar->m_alignment = bqGUIElement::Alignment::Left;*/
                sld->m_alignment = bqGUIElement::Alignment::Center;
                bqFramework::RebuildGUI();
#endif
                bqImage screenImg;
                screenImg.Create(800, 600);
                screenImg.Fill(bqImageFillType::Solid, bq::ColorWhite,
                    bq::ColorWhite);

                Renderer* mFiller = new PolygonVersionF();
                ((PolygonVersionF*)mFiller)->init(800, 600, 300);
                // ((PolygonVersionF*)mFiller)->setClipRect(0, 0, 800, 600);

                DefaultPolygonFactory factory;
                VectorGraphic* vg = TestPattern::circles(INT_TO_RATIONAL(108), FLOAT_TO_RATIONAL(1.0f), 100, 36,
                    VECTOR2D(INT_TO_RATIONAL(110), INT_TO_RATIONAL(110)),
                    RENDERER_FILLMODE_EVENODD, 0xff000000,
                    &factory,
                     NULL,NULL);
                
                vg = TestPattern::star(INT_TO_RATIONAL(110), 5,
                    VECTOR2D(INT_TO_RATIONAL(110), INT_TO_RATIONAL(110)),
                    RENDERER_FILLMODE_EVENODD, 0xff000000,
                    &factory,
                   NULL,
                    0);
               
                
                BitmapData bitmap(screenImg.m_info.m_width, 
                    screenImg.m_info.m_height,
                    screenImg.m_info.m_pitch, 
                    (unsigned long*)screenImg.m_data, BitmapData::BITMAP_FORMAT_XRGB);
                MATRIX2D aTransformation;
                struct _tr
                {
                    void updateTransformation(MATRIX2D& aTransformation)
                    {
                        VECTOR2D negPivot(-VECTOR2D_GETX(mPivot), -VECTOR2D_GETY(mPivot));
                        MATRIX2D_MAKETRANSLATION(aTransformation, negPivot);
                        MATRIX2D_ROTATE(aTransformation, mCurrentRotation);
                        MATRIX2D_TRANSLATE(aTransformation, mPivot);
                        MATRIX2D_TRANSLATE(aTransformation, mCurrentTranslation);

                        VECTOR2D scale;
                        RATIONAL t = mScale;
                        VECTOR2D_SETX(scale, t);
                        VECTOR2D_SETY(scale, t);
                        MATRIX2D_SCALE(aTransformation, scale);

                        VECTOR2D_SETX(mCurrentTranslation, VECTOR2D_GETX(mCurrentTranslation) + VECTOR2D_GETX(mMovement));
                        VECTOR2D_SETY(mCurrentTranslation, VECTOR2D_GETY(mCurrentTranslation) + VECTOR2D_GETY(mMovement));
                    }

                    VECTOR2D mPivot;
                    float mScale = 2.f;
                    RATIONAL mCurrentRotation = 0;
                    VECTOR2D mCurrentTranslation;
                    VECTOR2D mMovement;
                }__tr;
                __tr.updateTransformation(aTransformation);

                vg->render(mFiller, &bitmap, aTransformation);

             //   memcpy(screenImg.m_data, bitmap.mData, screenImg.m_dataSize);
              //  screenImg.SaveToFile(bqImage::SaveFileFormat::ddsRGBA8, "testVG.dds");

                bqTexture* texture = gs->SummonTexture(&screenImg);

                {
               
                    float* dt_ptr = bqFramework::GetDeltaTime();
                    while (g_run)
                    {
                        bqFramework::Update();
#ifdef BQ_WITH_GUI
                        bqFramework::UpdateGUI();
#endif

                        if (bqInput::IsKeyHit(bqInput::KEY_1))
                        {
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_2))
                        {
                        }

                        
                        gs->BeginDraw();
                        gs->ClearAll();

                        gs->SetShader(bqShaderType::Line3D, 0);
                        gs->DrawLine3D(
                            bqVec4(10.f, 0.f, 0.f, 0.f),
                            bqVec4(-10.f, 0.f, 0.f, 0.f),
                            bqColor(1.f, 0.f, 0.f, 1.f));

                        gs->DrawLine3D(
                            bqVec4(0.f, 0.f, 10.f, 0.f),
                            bqVec4(0.f, 0.f, -10.f, 0.f),
                            bqColor(0.f, 1.f, 0.f, 1.f));

                        gs->SetShader(bqShaderType::Standart, 0);

                        bqMat4 WorldViewProjection;
                        bqMat4 World;

                        static float angle = 0.f;
                        World.SetRotation(bqQuaternion(0.f, angle, 0.f));
                        angle += 0.01f;
                        if (angle > PIPI)
                            angle = 0.f;
                        WorldViewProjection = camera.GetMatrixProjection() * camera.GetMatrixView() * World;
                        bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WorldViewProjection);
                        bqFramework::SetMatrix(bqMatrixType::World, &World);

                        
#ifdef BQ_WITH_GUI
                        gs->BeginGUI();
                        gs->DrawGUIRectangle(bqVec4f(0.f, 0.f, 800.f, 600.f), bq::ColorWhite,
                            bq::ColorWhite, texture, 0);
                        //gs->DrawGUIText(U"Hello!!!", 9, bqVec2f(10.f), &tdcb);
                        bqFramework::DrawGUI(gs);
                        gs->EndGUI();
#endif

                        gs->EndDraw();
                        gs->SwapBuffers();
                    }
                }

                gs->Shutdown();
            }
        }

        delete window;
    }
    
    return EXIT_SUCCESS;
}
