/**
 * Component to allow for setting custom colors and effects
 * 
 * - Selected color and effect are pushed to /api/effects/set.
 */

import React, { Component } from "react";
import Typography from "@material-ui/core/Typography";
import { withStyles } from "@material-ui/core/styles";
import { HuePicker } from 'react-color';

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
          b: 0
        }
      };
    }
  
    // Set color and send to device
    handlePrimaryColorChange(color) {
      let r = color.rgb.r;
      let g = color.rgb.g;
      let b = color.rgb.b;

      fetch('/api/effects/set?r=' + r + '&g=' + g + '&b=' + b)
        .then((response) => response.json())
        .then((data) => 
          this.setState({ 
            selectedColor: color.rgb 
          })
      );
    };
  
    render() {
      return (
        <div>
          <WhiteTextTypography variant="h5" color="textSecondary" gutterBottom>
            Primary Color
          </WhiteTextTypography>
          <HuePicker color={ this.state.primaryColor } onChangeComplete={ this.handlePrimaryColorChange }/>
        </div>
        
      );
    }
  }
  
  export default EffectPicker;
  