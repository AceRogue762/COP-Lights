/**
 * Component to allow for setting custom colors and effects
 * 
 * - Selected color and effect are pushed to /api/effects/set.
 */

import React, { Component } from "react";
import { HuePicker, AlphaPicker } from 'react-color';
import WhiteTextTypography from './WhiteTextTypography';

class EffectPicker extends Component {
    constructor() {
      super();
  
      this.state = {
        primaryColor: {
          r: 0, 
          g: 0, 
          b: 0, 
          a: 1
        }
      };
    }

    // Set primary color state
    handlePrimaryColorChange = (color) => {
      let r = color.rgb.r;
      let g = color.rgb.g;
      let b = color.rgb.b;
      let a = this.state.primaryColor.a;

      this.setState({ 
        primaryColor: {
          r: r, 
          g: g, 
          b: b, 
          a: a
        }
      });
    };

    // Send color to device
    handlePrimaryColorChangeComplete = (color) => {
      let r = this.state.primaryColor.r;
      let g = this.state.primaryColor.g;
      let b = this.state.primaryColor.b;
      let a = this.state.primaryColor.a;

      // Scale RGB values by alpha
      let scaledR = r * a;
      let scaledG = g * a;
      let scaledB = b * a;

      // Send color to API
      fetch(`/api/effects/set?r=${scaledR}&g=${scaledG}&b=${scaledB}`);
    };

    // Set primary color alpha value
    handlePrimaryAlphaChange = (color) => {
      let r = this.state.primaryColor.r;
      let g = this.state.primaryColor.g;
      let b = this.state.primaryColor.b;
      let a = color.rgb.a;

      this.setState({ 
        primaryColor: {
          r: r, 
          g: g, 
          b: b, 
          a: a
        }
      });
    }

    render() {
      return (
        <div>
          <WhiteTextTypography variant="h5" color="textSecondary" gutterBottom>
            Primary Color
          </WhiteTextTypography>
          <HuePicker 
            color={ this.state.primaryColor } 
            onChange={ this.handlePrimaryColorChange }
            onChangeComplete={ this.handlePrimaryColorChangeComplete }
          />
          <AlphaPicker 
            color={ this.state.primaryColor }
            onChange={ this.handlePrimaryAlphaChange }
            onChangeComplete={ this.handlePrimaryColorChangeComplete }
          />
        </div>
      );
    }
  }
  
  export default EffectPicker;
  