add_library(keywordtypes
  analyser.cpp
  atomtypeselection.cpp
  bool.cpp
  broadeningfunction.cpp
  charstring.cpp
  complex.cpp
  datastore.cpp
  double.cpp
  enumstring.cpp
  fileandformat.cpp
  int.cpp
  isotopologuelist.cpp
  modulegroups.cpp
  modulereferencelist.cpp
  pairbroadeningfunction.cpp
  speciesreferencelist.cpp
  speciessite.cpp
  windowfunction.cpp

  analyser.h
  atomtypeselection.h
  bool.h
  broadeningfunction.h
  charstring.h
  complex.h
  datastore.h
  double.h
  enumstring.h
  fileandformat.h
  int.h
  isotopologuelist.h
  modulegroups.h
  modulereferencelist.h
  pairbroadeningfunction.h
  speciesreferencelist.h
  speciessite.h
  windowfunction.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)

