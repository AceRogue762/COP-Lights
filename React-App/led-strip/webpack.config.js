/**
 * Webpack configuration for the application
 * - Using babel for js/jsx
 * - Using html-loader for html
 * - Using style-loader and css-loader for CSS
 * - Using html-webpack-plugin to generate static HTML
 *   (https://webpack.js.org/plugins/html-webpack-plugin/)
 * 
 * Change devServer.proxy to your device's URI for development.
 * For deployment, run 'npm run build' and copy dist/index.html and 
 * dist/main.js to your device.
 */

const HtmlWebPackPlugin = require("html-webpack-plugin");

module.exports = {
  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        exclude: /node_modules/,
        use: {
          loader: "babel-loader"
        }
      },
      
      {
        test: /\.html$/,
        use: [
          {
            loader: "html-loader"
          }
        ]
      }, 

      {
        test: /\.css$/i,
        use: ['style-loader', 'css-loader'],
      },
    ]
  },
  
  devServer: {
    proxy: {
      '/animations': 'http://192.168.1.25'
    }
  },

  plugins: [
    new HtmlWebPackPlugin({
      template: "./src/index.html",
      filename: "./index.html"
    })
  ]
};

