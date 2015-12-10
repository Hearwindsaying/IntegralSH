#pragma once

// Include Eigen
#include <Eigen/Core>

// Include STL
#include <fstream>
#include <string>
#include <vector>
#include <exception>

struct IOError : public std::exception {
   std::string _msg;

   IOError(const std::string& filename = "") {
      _msg = "Unable to open file descriptor \"";
      _msg.append(filename);
      _msg.append("\"");
   }

   virtual const char* what() const throw() {
      return _msg.c_str();
   }
};

/* Enable to load a list of Eigen Matrices to binary format
 */
inline std::vector<Eigen::MatrixXf> LoadMatrices(const std::string& filename) {
   // Open file
   auto out = std::fopen(filename.c_str(), "r");
   if(out == nullptr) {
      IOError ioException(filename);
      throw ioException;
   }

   // Load the number of matrices
   int size;
   std::fread(&size, sizeof(int), 1, out);
   std::vector<Eigen::MatrixXf> matrices(size);

   for(int k=0; k<size; ++k) {
      Eigen::MatrixXf& mat = matrices[k];

      int nrows;
      int ncols;
      std::fread(&nrows, sizeof(int), 1, out);
      std::fread(&ncols, sizeof(int), 1, out);
      mat = Eigen::MatrixXf(nrows, ncols);
      std::fread(mat.data(), sizeof(float), nrows*ncols, out);
   }
   std::fclose(out);

   return matrices;
}

/* Enable to save a list of Eigen Matrices from a binary format
 */
inline void SaveMatrices(const std::string& filename,
                         const std::vector<Eigen::MatrixXf>& matrices) {

   // File descriptor
   auto out = std::fopen(filename.c_str(), "w");

   // Output list of matrices
   const int size = matrices.size();
   std::fwrite(&size, sizeof(int), 1, out);
   for(const auto& mat : matrices) {
      // Output the matrix size
      const int nrows = mat.rows();
      const int ncols =  mat.cols();
      std::fwrite(&nrows, sizeof(int), 1, out);
      std::fwrite(&ncols, sizeof(int), 1, out);
      std::fwrite(mat.data(), sizeof(float), nrows*ncols, out);
   }
   std::fclose(out);
}