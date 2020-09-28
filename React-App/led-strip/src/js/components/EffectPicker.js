/**
 * Component to allow for setting custom colors and effects
 * 
 * - Selected color and effect are pushed to /api/effects/set.
 */

import React, { Component } from "react";
import Typography from "@material-ui/core/Typography";
import { withStyles } from "@material-ui/core/styles";
import { HuePicker, AlphaPicker } from 'react-color';

const WhiteTextTypography = withStyles({
    root: {
      color: "#FFFFFF"
    }
  })(Typography);

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

    // Set primary color and send to device
    handlePrimaryColorChange(color) {
      let a = color.rgb.a;

      // Scale RGB values according to alpha
      let r = color.rgb.r * a;
      let g = color.rgb.g * a;
      let b = color.rgb.b * a;

      fetch('/api/effects/set?r=' + r + '&g=' + g + '&b=' + b)
        .then((response) => response.json())
        .then((data) => 
          this.setState({ 
            primaryColor: color.rgb 
          })
      );
    };

    render() {
      return (
        <div>
          <WhiteTextTypography variant="h5" color="textSecondary" gutterBottom>
            Primary Color
          </WhiteTextTypography>
          <HuePicker 
            color={ this.state.primaryColor } 
            onChangeComplete={ this.handlePrimaryColorChange }
          />
          <AlphaPicker 
            color={ this.state.primaryColor }
            onChangeComplete={ this.handlePrimaryColorChange }
          />
        </div>
        
      );
    }
  }
  
  export default EffectPicker;
  