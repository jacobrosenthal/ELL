#
# cmake file
#

set (library_name utilities)

set (src src/Archiver.cpp
         src/CommandLineParser.cpp
         src/CompressedIntegerList.cpp
         src/ConformingVector.cpp
         src/Files.cpp
         src/Format.cpp
         src/IArchivable.cpp
         src/IndentedTextWriter.cpp
         src/IntegerList.cpp
         src/IntegerStack.cpp
         src/JsonArchiver.cpp
         src/ObjectArchive.cpp
         src/ObjectArchiver.cpp
         src/OutputStreamImpostor.cpp
         src/RandomEngines.cpp
         src/Tokenizer.cpp
         src/TypeName.cpp
         src/UniqueId.cpp
         src/Variant.cpp
         src/XmlArchiver.cpp)

set (include include/AbstractInvoker.h
             include/AnyIterator.h
             include/Archiver.h
             include/CommandLineParser.h
             include/CompressedIntegerList.h
             include/ConformingVector.h
             include/DynamicArray.h
             include/Exception.h
             include/Files.h
             include/Format.h
             include/FunctionUtils.h
             include/IArchivable.h
             include/IIterator.h
             include/IndentedTextWriter.h
             include/IntegerList.h
             include/IntegerStack.h
             include/JsonArchiver.h
             include/ObjectArchive.h
             include/ObjectArchiver.h
             include/OutputStreamImpostor.h
             include/ParallelTransformIterator.h
             include/Parser.h
             include/RandomEngines.h
             include/StlReferenceIterator.h
             include/Tokenizer.h
             include/TransformIterator.h
             include/TupleWrapper.h
             include/TypeFactory.h
             include/TypeName.h
             include/TypeTraits.h
             include/UniqueId.h
             include/Variant.h
             include/XmlArchiver.h)

set (tcc tcc/AbstractInvoker.tcc
         tcc/AnyIterator.tcc
         tcc/Archiver.tcc
         tcc/CommandLineParser.tcc
         tcc/DynamicArray.tcc
         tcc/Exception.tcc
         tcc/Format.tcc
         tcc/FunctionUtils.tcc
         tcc/IArchivable.tcc
         tcc/JsonArchiver.tcc
         tcc/ObjectArchive.tcc
         tcc/ObjectArchiver.tcc
         tcc/OutputStreamImpostor.tcc
         tcc/ParallelTransformIterator.tcc
         tcc/Parser.tcc
         tcc/StlReferenceIterator.tcc
         tcc/TransformIterator.tcc
         tcc/TypeFactory.tcc
         tcc/TypeName.tcc
         tcc/TypeTraits.tcc
         tcc/Variant.tcc
         tcc/XmlArchiver.tcc)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)

if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(${library_name} -pthread)
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test projects
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp 
              test/src/IArchivable_test.cpp
              test/src/Iterator_test.cpp
              test/src/ObjectArchive_test.cpp
              test/src/TypeFactory_test.cpp
              test/src/Variant_test.cpp)

set (test_include test/include/IArchivable_test.h
                  test/include/Iterator_test.h
                  test/include/ObjectArchive_test.h
                  test/src/TypeFactory_test.cpp
                  test/src/Variant_test.cpp)
                  
source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)

target_link_libraries(${test_name} utilities testing data model nodes)
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(${test_name} -pthread)
endif()

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")
add_test(NAME ${test_name} COMMAND ${test_name})

