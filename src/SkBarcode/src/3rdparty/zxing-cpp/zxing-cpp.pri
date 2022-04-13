# zxing-cpp module

INCLUDEPATH += src/3rdparty/zxing-cpp \

HEADERS += src/3rdparty/zxing-cpp/BarcodeFormat.h \
           src/3rdparty/zxing-cpp/BinaryBitmap.h \
           src/3rdparty/zxing-cpp/BitArray.h \
           src/3rdparty/zxing-cpp/BitHacks.h \
           src/3rdparty/zxing-cpp/BitMatrix.h \
           src/3rdparty/zxing-cpp/BitMatrixCursor.h \
           src/3rdparty/zxing-cpp/BitMatrixIO.h \
           src/3rdparty/zxing-cpp/BitSource.h \
           src/3rdparty/zxing-cpp/ByteArray.h \
           src/3rdparty/zxing-cpp/ByteMatrix.h \
           src/3rdparty/zxing-cpp/CharacterSet.h \
           src/3rdparty/zxing-cpp/CharacterSetECI.h \
           src/3rdparty/zxing-cpp/ConcentricFinder.h \
           src/3rdparty/zxing-cpp/CustomData.h \
           src/3rdparty/zxing-cpp/DecodeHints.h \
           src/3rdparty/zxing-cpp/DecodeStatus.h \
           src/3rdparty/zxing-cpp/DecoderResult.h \
           src/3rdparty/zxing-cpp/DetectorResult.h \
           src/3rdparty/zxing-cpp/Flags.h \
           src/3rdparty/zxing-cpp/GTIN.h \
           src/3rdparty/zxing-cpp/GenericGF.h \
           src/3rdparty/zxing-cpp/GenericGFPoly.h \
           src/3rdparty/zxing-cpp/GenericLuminanceSource.h \
           src/3rdparty/zxing-cpp/GlobalHistogramBinarizer.h \
           src/3rdparty/zxing-cpp/GridSampler.h \
           src/3rdparty/zxing-cpp/HybridBinarizer.h \
           src/3rdparty/zxing-cpp/LogMatrix.h \
           src/3rdparty/zxing-cpp/LuminanceSource.h \
           src/3rdparty/zxing-cpp/Matrix.h \
           src/3rdparty/zxing-cpp/MultiFormatReader.h \
           src/3rdparty/zxing-cpp/MultiFormatWriter.h \
           src/3rdparty/zxing-cpp/Pattern.h \
           src/3rdparty/zxing-cpp/PerspectiveTransform.h \
           src/3rdparty/zxing-cpp/Point.h \
           src/3rdparty/zxing-cpp/Quadrilateral.h \
           src/3rdparty/zxing-cpp/ReadBarcode.h \
           src/3rdparty/zxing-cpp/Reader.h \
           src/3rdparty/zxing-cpp/ReedSolomonDecoder.h \
           src/3rdparty/zxing-cpp/ReedSolomonEncoder.h \
           src/3rdparty/zxing-cpp/RegressionLine.h \
           src/3rdparty/zxing-cpp/Result.h \
           src/3rdparty/zxing-cpp/ResultMetadata.h \
           src/3rdparty/zxing-cpp/ResultPoint.h \
           src/3rdparty/zxing-cpp/Scope.h \
           src/3rdparty/zxing-cpp/StructuredAppend.h \
           src/3rdparty/zxing-cpp/TextDecoder.h \
           src/3rdparty/zxing-cpp/TextEncoder.h \
           src/3rdparty/zxing-cpp/TextUtfEncoding.h \
           src/3rdparty/zxing-cpp/ThresholdBinarizer.h \
           src/3rdparty/zxing-cpp/TritMatrix.h \
           src/3rdparty/zxing-cpp/WhiteRectDetector.h \
           src/3rdparty/zxing-cpp/ZXBigInteger.h \
           src/3rdparty/zxing-cpp/ZXConfig.h \
           src/3rdparty/zxing-cpp/ZXContainerAlgorithms.h \
           src/3rdparty/zxing-cpp/ZXNullable.h \
           src/3rdparty/zxing-cpp/ZXTestSupport.h \
           src/3rdparty/zxing-cpp/aztec/AZDecoder.h \
           src/3rdparty/zxing-cpp/aztec/AZDetector.h \
           src/3rdparty/zxing-cpp/aztec/AZDetectorResult.h \
           src/3rdparty/zxing-cpp/aztec/AZEncoder.h \
           src/3rdparty/zxing-cpp/aztec/AZEncodingState.h \
           src/3rdparty/zxing-cpp/aztec/AZHighLevelEncoder.h \
           src/3rdparty/zxing-cpp/aztec/AZReader.h \
           src/3rdparty/zxing-cpp/aztec/AZToken.h \
           src/3rdparty/zxing-cpp/aztec/AZWriter.h \
           src/3rdparty/zxing-cpp/datamatrix/DMBitLayout.h \
           src/3rdparty/zxing-cpp/datamatrix/DMDataBlock.h \
           src/3rdparty/zxing-cpp/datamatrix/DMDecoder.h \
           src/3rdparty/zxing-cpp/datamatrix/DMDetector.h \
           src/3rdparty/zxing-cpp/datamatrix/DMECEncoder.h \
           src/3rdparty/zxing-cpp/datamatrix/DMEncoderContext.h \
           src/3rdparty/zxing-cpp/datamatrix/DMHighLevelEncoder.h \
           src/3rdparty/zxing-cpp/datamatrix/DMReader.h \
           src/3rdparty/zxing-cpp/datamatrix/DMSymbolInfo.h \
           src/3rdparty/zxing-cpp/datamatrix/DMSymbolShape.h \
           src/3rdparty/zxing-cpp/datamatrix/DMVersion.h \
           src/3rdparty/zxing-cpp/datamatrix/DMWriter.h \
           src/3rdparty/zxing-cpp/maxicode/MCBitMatrixParser.h \
           src/3rdparty/zxing-cpp/maxicode/MCDecoder.h \
           src/3rdparty/zxing-cpp/maxicode/MCReader.h \
           src/3rdparty/zxing-cpp/oned/ODCodabarReader.h \
           src/3rdparty/zxing-cpp/oned/ODCodabarWriter.h \
           src/3rdparty/zxing-cpp/oned/ODCode128Patterns.h \
           src/3rdparty/zxing-cpp/oned/ODCode128Reader.h \
           src/3rdparty/zxing-cpp/oned/ODCode128Writer.h \
           src/3rdparty/zxing-cpp/oned/ODCode39Reader.h \
           src/3rdparty/zxing-cpp/oned/ODCode39Writer.h \
           src/3rdparty/zxing-cpp/oned/ODCode93Reader.h \
           src/3rdparty/zxing-cpp/oned/ODCode93Writer.h \
           src/3rdparty/zxing-cpp/oned/ODDataBarCommon.h \
           src/3rdparty/zxing-cpp/oned/ODDataBarExpandedReader.h \
           src/3rdparty/zxing-cpp/oned/ODDataBarReader.h \
           src/3rdparty/zxing-cpp/oned/ODEAN13Writer.h \
           src/3rdparty/zxing-cpp/oned/ODEAN8Writer.h \
           src/3rdparty/zxing-cpp/oned/ODITFReader.h \
           src/3rdparty/zxing-cpp/oned/ODITFWriter.h \
           src/3rdparty/zxing-cpp/oned/ODMultiUPCEANReader.h \
           src/3rdparty/zxing-cpp/oned/ODReader.h \
           src/3rdparty/zxing-cpp/oned/ODRowReader.h \
           src/3rdparty/zxing-cpp/oned/ODUPCAWriter.h \
           src/3rdparty/zxing-cpp/oned/ODUPCEANCommon.h \
           src/3rdparty/zxing-cpp/oned/ODUPCEWriter.h \
           src/3rdparty/zxing-cpp/oned/ODWriterHelper.h \
           src/3rdparty/zxing-cpp/oned/rss/ODRSSExpandedBinaryDecoder.h \
           src/3rdparty/zxing-cpp/oned/rss/ODRSSFieldParser.h \
           src/3rdparty/zxing-cpp/oned/rss/ODRSSGenericAppIdDecoder.h \
           src/3rdparty/zxing-cpp/pdf417/PDFBarcodeMetadata.h \
           src/3rdparty/zxing-cpp/pdf417/PDFBarcodeValue.h \
           src/3rdparty/zxing-cpp/pdf417/PDFBoundingBox.h \
           src/3rdparty/zxing-cpp/pdf417/PDFCodeword.h \
           src/3rdparty/zxing-cpp/pdf417/PDFCodewordDecoder.h \
           src/3rdparty/zxing-cpp/pdf417/PDFCompaction.h \
           src/3rdparty/zxing-cpp/pdf417/PDFDecodedBitStreamParser.h \
           src/3rdparty/zxing-cpp/pdf417/PDFDecoderResultExtra.h \
           src/3rdparty/zxing-cpp/pdf417/PDFDetectionResult.h \
           src/3rdparty/zxing-cpp/pdf417/PDFDetectionResultColumn.h \
           src/3rdparty/zxing-cpp/pdf417/PDFDetector.h \
           src/3rdparty/zxing-cpp/pdf417/PDFEncoder.h \
           src/3rdparty/zxing-cpp/pdf417/PDFHighLevelEncoder.h \
           src/3rdparty/zxing-cpp/pdf417/PDFModulusGF.h \
           src/3rdparty/zxing-cpp/pdf417/PDFModulusPoly.h \
           src/3rdparty/zxing-cpp/pdf417/PDFReader.h \
           src/3rdparty/zxing-cpp/pdf417/PDFScanningDecoder.h \
           src/3rdparty/zxing-cpp/pdf417/PDFWriter.h \
           src/3rdparty/zxing-cpp/qrcode/QRBitMatrixParser.h \
           src/3rdparty/zxing-cpp/qrcode/QRCodecMode.h \
           src/3rdparty/zxing-cpp/qrcode/QRDataBlock.h \
           src/3rdparty/zxing-cpp/qrcode/QRDataMask.h \
           src/3rdparty/zxing-cpp/qrcode/QRDecoder.h \
           src/3rdparty/zxing-cpp/qrcode/QRDecoderMetadata.h \
           src/3rdparty/zxing-cpp/qrcode/QRDetector.h \
           src/3rdparty/zxing-cpp/qrcode/QRECB.h \
           src/3rdparty/zxing-cpp/qrcode/QREncodeResult.h \
           src/3rdparty/zxing-cpp/qrcode/QREncoder.h \
           src/3rdparty/zxing-cpp/qrcode/QRErrorCorrectionLevel.h \
           src/3rdparty/zxing-cpp/qrcode/QRFormatInformation.h \
           src/3rdparty/zxing-cpp/qrcode/QRMaskUtil.h \
           src/3rdparty/zxing-cpp/qrcode/QRMatrixUtil.h \
           src/3rdparty/zxing-cpp/qrcode/QRReader.h \
           src/3rdparty/zxing-cpp/qrcode/QRVersion.h \
           src/3rdparty/zxing-cpp/qrcode/QRWriter.h \
           src/3rdparty/zxing-cpp/textcodec/Big5MapTable.h \
           src/3rdparty/zxing-cpp/textcodec/Big5TextDecoder.h \
           src/3rdparty/zxing-cpp/textcodec/Big5TextEncoder.h \
           src/3rdparty/zxing-cpp/textcodec/GBTextDecoder.h \
           src/3rdparty/zxing-cpp/textcodec/GBTextEncoder.h \
           src/3rdparty/zxing-cpp/textcodec/JPTextDecoder.h \
           src/3rdparty/zxing-cpp/textcodec/JPTextEncoder.h \
           src/3rdparty/zxing-cpp/textcodec/KRHangulMapping.h \
           src/3rdparty/zxing-cpp/textcodec/KRTextDecoder.h \
           src/3rdparty/zxing-cpp/textcodec/KRTextEncoder.h \

SOURCES += src/3rdparty/zxing-cpp/BarcodeFormat.cpp \
           src/3rdparty/zxing-cpp/BinaryBitmap.cpp \
           src/3rdparty/zxing-cpp/BitArray.cpp \
           src/3rdparty/zxing-cpp/BitMatrix.cpp \
           src/3rdparty/zxing-cpp/BitMatrixIO.cpp \
           src/3rdparty/zxing-cpp/BitSource.cpp \
           src/3rdparty/zxing-cpp/CharacterSetECI.cpp \
           src/3rdparty/zxing-cpp/ConcentricFinder.cpp \
           src/3rdparty/zxing-cpp/DecodeHints.cpp \
           src/3rdparty/zxing-cpp/DecodeStatus.cpp \
           src/3rdparty/zxing-cpp/GTIN.cpp \
           src/3rdparty/zxing-cpp/GenericGF.cpp \
           src/3rdparty/zxing-cpp/GenericGFPoly.cpp \
           src/3rdparty/zxing-cpp/GenericLuminanceSource.cpp \
           src/3rdparty/zxing-cpp/GlobalHistogramBinarizer.cpp \
           src/3rdparty/zxing-cpp/GridSampler.cpp \
           src/3rdparty/zxing-cpp/HybridBinarizer.cpp \
           src/3rdparty/zxing-cpp/LuminanceSource.cpp \
           src/3rdparty/zxing-cpp/MultiFormatReader.cpp \
           src/3rdparty/zxing-cpp/MultiFormatWriter.cpp \
           src/3rdparty/zxing-cpp/PerspectiveTransform.cpp \
           src/3rdparty/zxing-cpp/ReadBarcode.cpp \
           src/3rdparty/zxing-cpp/ReedSolomonDecoder.cpp \
           src/3rdparty/zxing-cpp/ReedSolomonEncoder.cpp \
           src/3rdparty/zxing-cpp/Result.cpp \
           src/3rdparty/zxing-cpp/ResultMetadata.cpp \
           src/3rdparty/zxing-cpp/ResultPoint.cpp \
           src/3rdparty/zxing-cpp/TextDecoder.cpp \
           src/3rdparty/zxing-cpp/TextEncoder.cpp \
           src/3rdparty/zxing-cpp/TextUtfEncoding.cpp \
           src/3rdparty/zxing-cpp/WhiteRectDetector.cpp \
           src/3rdparty/zxing-cpp/ZXBigInteger.cpp \
           src/3rdparty/zxing-cpp/aztec/AZDecoder.cpp \
           src/3rdparty/zxing-cpp/aztec/AZDetector.cpp \
           src/3rdparty/zxing-cpp/aztec/AZEncoder.cpp \
           src/3rdparty/zxing-cpp/aztec/AZHighLevelEncoder.cpp \
           src/3rdparty/zxing-cpp/aztec/AZReader.cpp \
           src/3rdparty/zxing-cpp/aztec/AZToken.cpp \
           src/3rdparty/zxing-cpp/aztec/AZWriter.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMBitLayout.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMDataBlock.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMDecoder.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMDetector.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMECEncoder.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMHighLevelEncoder.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMReader.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMSymbolInfo.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMVersion.cpp \
           src/3rdparty/zxing-cpp/datamatrix/DMWriter.cpp \
           src/3rdparty/zxing-cpp/maxicode/MCBitMatrixParser.cpp \
           src/3rdparty/zxing-cpp/maxicode/MCDecoder.cpp \
           src/3rdparty/zxing-cpp/maxicode/MCReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODCodabarReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODCodabarWriter.cpp \
           src/3rdparty/zxing-cpp/oned/ODCode128Patterns.cpp \
           src/3rdparty/zxing-cpp/oned/ODCode128Reader.cpp \
           src/3rdparty/zxing-cpp/oned/ODCode128Writer.cpp \
           src/3rdparty/zxing-cpp/oned/ODCode39Reader.cpp \
           src/3rdparty/zxing-cpp/oned/ODCode39Writer.cpp \
           src/3rdparty/zxing-cpp/oned/ODCode93Reader.cpp \
           src/3rdparty/zxing-cpp/oned/ODCode93Writer.cpp \
           src/3rdparty/zxing-cpp/oned/ODDataBarCommon.cpp \
           src/3rdparty/zxing-cpp/oned/ODDataBarExpandedReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODDataBarReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODEAN13Writer.cpp \
           src/3rdparty/zxing-cpp/oned/ODEAN8Writer.cpp \
           src/3rdparty/zxing-cpp/oned/ODITFReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODITFWriter.cpp \
           src/3rdparty/zxing-cpp/oned/ODMultiUPCEANReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODRowReader.cpp \
           src/3rdparty/zxing-cpp/oned/ODUPCAWriter.cpp \
           src/3rdparty/zxing-cpp/oned/ODUPCEANCommon.cpp \
           src/3rdparty/zxing-cpp/oned/ODUPCEWriter.cpp \
           src/3rdparty/zxing-cpp/oned/ODWriterHelper.cpp \
           src/3rdparty/zxing-cpp/oned/rss/ODRSSExpandedBinaryDecoder.cpp \
           src/3rdparty/zxing-cpp/oned/rss/ODRSSFieldParser.cpp \
           src/3rdparty/zxing-cpp/oned/rss/ODRSSGenericAppIdDecoder.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFBarcodeValue.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFBoundingBox.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFCodewordDecoder.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFDecodedBitStreamParser.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFDetectionResult.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFDetectionResultColumn.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFDetector.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFEncoder.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFHighLevelEncoder.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFModulusGF.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFModulusPoly.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFReader.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFScanningDecoder.cpp \
           src/3rdparty/zxing-cpp/pdf417/PDFWriter.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRBitMatrixParser.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRCodecMode.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRDataBlock.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRDecoder.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRDetector.cpp \
           src/3rdparty/zxing-cpp/qrcode/QREncoder.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRErrorCorrectionLevel.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRFormatInformation.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRMaskUtil.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRMatrixUtil.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRReader.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRVersion.cpp \
           src/3rdparty/zxing-cpp/qrcode/QRWriter.cpp \
           src/3rdparty/zxing-cpp/textcodec/Big5MapTable.cpp \
           src/3rdparty/zxing-cpp/textcodec/Big5TextDecoder.cpp \
           src/3rdparty/zxing-cpp/textcodec/Big5TextEncoder.cpp \
           src/3rdparty/zxing-cpp/textcodec/GBTextDecoder.cpp \
           src/3rdparty/zxing-cpp/textcodec/GBTextEncoder.cpp \
           src/3rdparty/zxing-cpp/textcodec/JPTextDecoder.cpp \
           src/3rdparty/zxing-cpp/textcodec/JPTextEncoder.cpp \
           src/3rdparty/zxing-cpp/textcodec/KRHangulMapping.cpp \
           src/3rdparty/zxing-cpp/textcodec/KRTextDecoder.cpp \
           src/3rdparty/zxing-cpp/textcodec/KRTextEncoder.cpp \
