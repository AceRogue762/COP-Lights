/**
 * Webpack configuration for the application
 * - Using babel for js/jsx
 * - Using html-loader for html
 * - Using style-loader and css-loader for CSS
 * - Using html-webpack-plugin to generate static HTML
 *   (https://webpack.js.org/plugins/html-webpack-plugin/)
 * 
 * Change devServer.proxy to your device's URI for development.
 * 
 * For deployment, run 'npm run build'. This will build and bundle
 * the application and copy it to LEDStripDriver/data. Then, in the
 * Arduino IDE, use Tools > ESP32 Sketch Data Upload to upload the 
 * updated build to your device.
 */

const HtmlWebPackPlugin = require("html-webpack-plugin");

module.exports = {
  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        exclude: /node_modules/,
        use: {
          loader: "babel-loader", 
          options: {
            presets: [
              '@babel/preset-env',
              '@babel/react',
              {
                'plugins': [
                  '@babel/plugin-proposal-class-properties'
                ]
              }
            ]
          }
        }, 
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

      {
        test: /\.(woff|woff2|eot|ttf|svg)$/,
        loader: 'file-loader?name=fonts/[name].[ext]'
      }
    ]
  },
  
  devServer: {
    proxy: {
      '/api': 'http://192.168.1.34'
    }
  },

  plugins: [
    new HtmlWebPackPlugin({
      template: "./src/index.html",
      filename: "./index.html"
    })
  ]
};

